<#
.SYNOPSIS
  Download, build, and stage the Windows 3rd-party bundle for MySQL Workbench / MySQL Studio.

.DESCRIPTION
  This script is the Windows-side equivalent of build/linux/build_3rdparty.sh.
  It creates a bundle layout compatible with the existing Visual Studio project files:

    <bundle>\
      bin\
      include\
      lib\
      debug\lib\
      python\

  The default output is a sibling build folder outside the source tree:

    ..\bundle

  The script mixes:
  - direct source builds for MySQL, Connector/C++, SQLite, Python, and header-only deps
  - explicit source builds for the heavier native libraries used by the Windows bundle

.EXAMPLE
  pwsh ./build/windows/build_3rdparty.ps1

.EXAMPLE
  pwsh ./build/windows/build_3rdparty.ps1 -Only openssl,zlib,libxml2,cairo

.EXAMPLE
  pwsh ./build/windows/build_3rdparty.ps1 -BundleDir D:\develop\MySQL\wb_build\bundle -Clean -VerboseOutput
#>

[CmdletBinding()]
param(
    [string]$BundleDir,
    [switch]$Clean,
    [switch]$DownloadOnly,
    [switch]$BuildOnly,
    [switch]$VerboseOutput,
    [int]$Jobs = [Math]::Max([Environment]::ProcessorCount, 1),
    [string[]]$Only = @(),
    [ValidateSet("Visual Studio 17 2022")]
    [string]$Generator = "Visual Studio 17 2022"
)

Set-StrictMode -Version 3.0
$ErrorActionPreference = "Stop"

$script:ScriptDir = Split-Path -Parent $PSCommandPath
$script:ProjectRoot = [System.IO.Path]::GetFullPath((Join-Path $script:ScriptDir "..\.."))
$script:ProjectParent = Split-Path -Parent $script:ProjectRoot
if (-not $BundleDir) {
    $BundleDir = Join-Path $script:ProjectParent "bundle"
}
$script:BundleDir = [System.IO.Path]::GetFullPath($BundleDir)
$script:WorkRoot = Join-Path $script:BundleDir "_work"
$script:DownloadRoot = Join-Path $script:WorkRoot "downloads"
$script:SourceRoot = Join-Path $script:WorkRoot "source"
$script:BuildRoot = Join-Path $script:WorkRoot "build"
$script:StampRoot = Join-Path $script:WorkRoot "stamps"
$script:ToolsRoot = Join-Path $script:BundleDir "_tools"
$script:DepManifestPath = Join-Path $script:ScriptDir "libs.txt"
$script:VsEnvLoaded = $false
$script:StatusLineVisible = $false
$script:StatusPanelMaxLines = 14
$script:StatusPanelTitle = ""
$script:StatusPanelLines = @()
$script:StatusPanelPhysicalRows = 0
$script:DepManifest = @()
$script:SelectedOnly = @($Only | Where-Object { -not [string]::IsNullOrWhiteSpace($_) })
$script:ActiveProcesses = [System.Collections.Generic.List[System.Diagnostics.Process]]::new()

function Stop-AllActiveProcesses {
    $procs = @($script:ActiveProcesses)
    $script:ActiveProcesses.Clear()
    foreach ($proc in $procs) {
        try {
            if ($proc -and -not $proc.HasExited) {
                & taskkill /T /F /PID $proc.Id 2>$null | Out-Null
            }
        }
        catch {}
        finally {
            try { $proc.Dispose() } catch {}
        }
    }
}

function Write-Banner {
    Clear-StatusLine
    Write-Host ""
    Write-Host "+------------------------------------------------------------------+" -ForegroundColor DarkYellow
    Write-Host "|   MySQL Workbench / Studio - Windows 3rd-Party Bundle Builder    |" -ForegroundColor Gray
    Write-Host "+------------------------------------------------------------------+" -ForegroundColor DarkYellow
    Write-Host ""
}

function Get-ConsoleWidth {
    try {
        return [Math]::Max($Host.UI.RawUI.BufferSize.Width, 80)
    }
    catch {
        return 120
    }
}

function Get-PhysicalRowCount([string]$Text, [int]$Width) {
    if ([string]::IsNullOrEmpty($Text) -or $Width -le 0) {
        return 1
    }
    return [Math]::Max(1, [Math]::Ceiling($Text.Length / $Width))
}

function Render-StatusPanel {
    if (-not $script:StatusLineVisible) {
        return
    }

    $width = Get-ConsoleWidth
    $esc = [char]0x1b
    $clearLine = "${esc}[2K"
    $colorGray = "${esc}[90m"
    $colorYellow = "${esc}[33m"
    $colorReset = "${esc}[0m"

    # Erase previous render by moving up the exact physical rows we wrote last time
    $prevRows = $script:StatusPanelPhysicalRows
    $buf = [System.Text.StringBuilder]::new(4096)
    if ($prevRows -gt 0) {
        [void]$buf.Append("${esc}[${prevRows}A")
        for ($i = 0; $i -lt $prevRows; $i++) {
            [void]$buf.Append($clearLine)
            [void]$buf.AppendLine()
        }
        [void]$buf.Append("${esc}[${prevRows}A")
    }

    # Build new content — lines are truncated so 1 line = 1 physical row
    $titleText = ([string]$script:StatusPanelTitle -replace "\s+", " ").Trim()
    $visibleLines = @($script:StatusPanelLines)
    $newPhysicalRows = 1 + $visibleLines.Count

    # Write title (truncate with ... if needed)
    $maxLen = $width - 2
    if ($titleText.Length -gt $maxLen) {
        $titleText = $titleText.Substring(0, $maxLen - 3) + "..."
    }
    [void]$buf.Append($clearLine)
    [void]$buf.Append($colorYellow)
    [void]$buf.Append($titleText)
    [void]$buf.AppendLine($colorReset)

    # Write output lines (truncate with ... if needed)
    foreach ($vl in $visibleLines) {
        $lineText = "  " + $vl
        if ($lineText.Length -gt $maxLen) {
            $lineText = $lineText.Substring(0, $maxLen - 3) + "..."
        }
        [void]$buf.Append($clearLine)
        [void]$buf.Append($colorGray)
        [void]$buf.Append($lineText)
        [void]$buf.AppendLine($colorReset)
    }

    [Console]::Write($buf.ToString())
    $script:StatusPanelPhysicalRows = $newPhysicalRows
}

function Start-StatusPanel([string]$Title) {
    if ($script:StatusLineVisible) {
        $script:StatusPanelTitle = $Title
        Render-StatusPanel
        return
    }

    $script:StatusPanelTitle = $Title
    $script:StatusPanelLines = @()
    $script:StatusLineVisible = $true
    $script:StatusPanelPhysicalRows = 0
    Render-StatusPanel
}

function Update-StatusPanelTitle([string]$Title) {
    if (-not $script:StatusLineVisible) {
        return
    }

    $script:StatusPanelTitle = $Title
    Render-StatusPanel
}

function Push-StatusPanelLine([string]$Message) {
    if (-not $script:StatusLineVisible -or [string]::IsNullOrWhiteSpace($Message)) {
        return
    }

    $normalized = ([string]$Message -replace "\s+", " ").Trim()
    if ($script:StatusPanelLines -is [System.Collections.ArrayList]) {
        [void]$script:StatusPanelLines.Add($normalized)
    } else {
        $script:StatusPanelLines = [System.Collections.ArrayList]@($script:StatusPanelLines)
        [void]$script:StatusPanelLines.Add($normalized)
    }
    while ($script:StatusPanelLines.Count -gt $script:StatusPanelMaxLines) {
        $script:StatusPanelLines.RemoveAt(0)
    }

    Render-StatusPanel
}

function Clear-StatusLine {
    if (-not $script:StatusLineVisible) {
        return
    }

    $prevRows = $script:StatusPanelPhysicalRows
    if ($prevRows -gt 0) {
        $esc = [char]0x1b
        $clearLine = "${esc}[2K"
        $buf = [System.Text.StringBuilder]::new(256)
        [void]$buf.Append("${esc}[${prevRows}A")
        for ($i = 0; $i -lt $prevRows; $i++) {
            [void]$buf.Append($clearLine)
            [void]$buf.AppendLine()
        }
        [void]$buf.Append("${esc}[${prevRows}A")
        [Console]::Write($buf.ToString())
    }

    $script:StatusLineVisible = $false
    $script:StatusPanelTitle = ""
    $script:StatusPanelLines = @()
    $script:StatusPanelPhysicalRows = 0
}

function Write-Section([string]$Message) {
    Clear-StatusLine
    Write-Host ""
    Write-Host ("-- {0} --" -f $Message) -ForegroundColor Magenta
}

function Write-Info([string]$Message) {
    Clear-StatusLine
    Write-Host ("  [INFO] {0}" -f $Message) -ForegroundColor Cyan
}

function Write-Ok([string]$Message) {
    Clear-StatusLine
    Write-Host ("  [ OK ] {0}" -f $Message) -ForegroundColor Green
}

function Write-Warn([string]$Message) {
    Clear-StatusLine
    Write-Host ("  [WARN] {0}" -f $Message) -ForegroundColor Yellow
}

function Write-Fail([string]$Message) {
    Clear-StatusLine
    Write-Host ("  [FAIL] {0}" -f $Message) -ForegroundColor Red
}

function Write-Detail([string]$Message) {
    Clear-StatusLine
    Write-Host ("    {0}" -f $Message) -ForegroundColor DarkGray
}

function Write-StepHeader([int]$Index, [int]$Total, [string]$Label) {
    $barLength = 30
    $filled = if ($Total -gt 0) { [Math]::Floor(($Index / $Total) * $barLength) } else { 0 }
    $filled = [Math]::Min($filled, $barLength)
    $bar = ("#" * $filled).PadRight($barLength, ".")
    Clear-StatusLine
    Write-Host ""
    Write-Host ("[{0}/{1}] {2}  {3}" -f $Index, $Total, $bar, $Label) -ForegroundColor White
}

function Get-RecentProcessLines([string[]]$Paths, [int]$TailCount = 50) {
    $noisePatterns = @(
        '^\s*$',
        '^Copyright \(C\) Microsoft Corporation\.',
        '^Microsoft \(R\).+',
        '^The input line is too long\.$',
        '^The syntax of the command is incorrect\.$',
        '^All rights reserved\.$'
    )

    $result = [System.Collections.Generic.List[string]]::new()
    foreach ($path in $Paths) {
        if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
            continue
        }

        $tail = @(Get-Content -LiteralPath $path -Tail $TailCount -ErrorAction SilentlyContinue)
        foreach ($line in $tail) {
            $text = ([string]$line).Trim()
            if ([string]::IsNullOrWhiteSpace($text)) { continue }
            $isNoise = $false
            foreach ($pattern in $noisePatterns) {
                if ($text -match $pattern) {
                    $isNoise = $true
                    break
                }
            }
            if (-not $isNoise) {
                $result.Add($text)
            }
        }
    }

    return $result
}

function Remove-FileWithRetry {
    param(
        [Parameter(Mandatory)] [string]$Path,
        [int]$Attempts = 20,
        [int]$DelayMilliseconds = 250
    )

    for ($attempt = 1; $attempt -le $Attempts; $attempt++) {
        if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
            return
        }

        try {
            Remove-Item -LiteralPath $Path -Force
            return
        }
        catch {
            if ($attempt -eq $Attempts) {
                Write-Warn ("Leaving temporary log file behind because it is still locked: {0}" -f $Path)
                return
            }
            Start-Sleep -Milliseconds $DelayMilliseconds
        }
    }
}

function Ensure-Directory([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Container)) {
        [void](New-Item -ItemType Directory -Path $Path -Force)
    }
}

function Reset-Directory([string]$Path) {
    if (Test-Path -LiteralPath $Path -PathType Container) {
        Remove-Item -LiteralPath $Path -Recurse -Force
    }
    [void](New-Item -ItemType Directory -Path $Path -Force)
}

function Initialize-BundleLayout {
    $dirs = @(
        $script:BundleDir,
        (Join-Path $script:BundleDir "bin"),
        (Join-Path $script:BundleDir "include"),
        (Join-Path $script:BundleDir "lib"),
        (Join-Path $script:BundleDir "debug"),
        (Join-Path $script:BundleDir "debug\lib"),
        (Join-Path $script:BundleDir "python"),
        (Join-Path $script:BundleDir "share"),
        $script:WorkRoot,
        $script:DownloadRoot,
        $script:SourceRoot,
        $script:BuildRoot,
        $script:StampRoot,
        $script:ToolsRoot
    )

    foreach ($dir in $dirs) {
        Ensure-Directory $dir
    }
}

function Get-DependencySlug([pscustomobject]$Dependency) {
    return "{0}-{1}" -f $Dependency.Name, $Dependency.Version
}

function Get-ArchivePath([pscustomobject]$Dependency) {
    if (-not $Dependency.PSObject.Properties["Urls"] -and -not $Dependency.PSObject.Properties["Url"]) {
        return $null
    }

    $primarySource = if ($Dependency.PSObject.Properties["Urls"] -and $Dependency.Urls.Count -gt 0) {
        $Dependency.Urls[0]
    }
    else {
        $Dependency.Url
    }

    $fileName = if ($primarySource -match '^[a-zA-Z][a-zA-Z0-9+.-]*://') {
        [System.IO.Path]::GetFileName(([Uri]$primarySource).AbsolutePath)
    }
    else {
        [System.IO.Path]::GetFileName($primarySource)
    }
    return Join-Path $script:DownloadRoot $fileName
}

function Get-ExtractPath([pscustomobject]$Dependency) {
    return Join-Path $script:SourceRoot (Get-DependencySlug $Dependency)
}

function Get-StampPath([pscustomobject]$Dependency) {
    return Join-Path $script:StampRoot ((Get-DependencySlug $Dependency) + ".done")
}

function Test-DependencyBuilt([pscustomobject]$Dependency) {
    return Test-Path -LiteralPath (Get-StampPath $Dependency)
}

function Mark-DependencyBuilt([pscustomobject]$Dependency) {
    Set-Content -LiteralPath (Get-StampPath $Dependency) -Value ("built {0:yyyy-MM-dd HH:mm:ss}" -f (Get-Date))
}

function Load-DependencyManifest {
    if (-not (Test-Path -LiteralPath $script:DepManifestPath -PathType Leaf)) {
        throw ("Dependency manifest not found: {0}" -f $script:DepManifestPath)
    }

    $dependencies = [System.Collections.Generic.List[pscustomobject]]::new()
    $seenNames = @{}
    $allowedTypes = @(
        "openssl",
        "zlib",
        "libxml2",
        "cairo",
        "libzip",
        "antlr4",
        "libssh",
        "proj",
        "gdal",
        "header-only",
        "sqlite",
        "vsqlitepp",
        "python",
        "mysql",
        "connector-cpp"
    )

    $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $script:DepManifestPath) {
        $lineNumber++
        $trimmed = $line.Trim()
        if (-not $trimmed -or $trimmed.StartsWith("#")) {
            continue
        }

        $parts = @($line.Split("|"))
        if ($parts.Count -ne 4) {
            throw ("Invalid dependency manifest entry at {0}:{1}. Expected format: name|version|type|url" -f $script:DepManifestPath, $lineNumber)
        }

        $name = $parts[0].Trim()
        $version = $parts[1].Trim()
        $type = $parts[2].Trim()
        $urlField = $parts[3].Trim()

        if ([string]::IsNullOrWhiteSpace($name) -or
            [string]::IsNullOrWhiteSpace($version) -or
            [string]::IsNullOrWhiteSpace($type) -or
            [string]::IsNullOrWhiteSpace($urlField)) {
            throw ("Invalid dependency manifest entry at {0}:{1}. None of the four fields may be empty." -f $script:DepManifestPath, $lineNumber)
        }

        $urls = @($urlField.Split(";") | ForEach-Object { $_.Trim() } | Where-Object { -not [string]::IsNullOrWhiteSpace($_) })
        if ($urls.Count -eq 0) {
            throw ("Invalid dependency manifest entry at {0}:{1}. At least one download source is required." -f $script:DepManifestPath, $lineNumber)
        }

        if ($type -notin $allowedTypes) {
            throw ("Unsupported dependency type '{0}' at {1}:{2}" -f $type, $script:DepManifestPath, $lineNumber)
        }

        if ($seenNames.ContainsKey($name)) {
            throw ("Duplicate dependency name '{0}' in {1} at lines {2} and {3}" -f $name, $script:DepManifestPath, $seenNames[$name], $lineNumber)
        }

        $seenNames[$name] = $lineNumber
        $dependencies.Add([pscustomobject]@{
            Name = $name
            Version = $version
            Type = $type
            Url = $urls[0]
            Urls = $urls
        })
    }

    if ($dependencies.Count -eq 0) {
        throw ("Dependency manifest is empty: {0}" -f $script:DepManifestPath)
    }

    return $dependencies
}

function Test-CommandAvailable([string]$Name) {
    return $null -ne (Get-Command $Name -ErrorAction SilentlyContinue)
}

function Find-VSWhere {
    $cmd = Get-Command "vswhere.exe" -ErrorAction SilentlyContinue
    if ($cmd) {
        return $cmd.Source
    }

    $fallback = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path -LiteralPath $fallback) {
        return $fallback
    }

    throw "vswhere.exe was not found."
}

function Get-VSInstallPath {
    $vsWhere = Find-VSWhere
    $path = (& $vsWhere -latest -version "[17,18)" -requires Microsoft.Component.MSBuild -property installationPath).Trim()
    if (-not $path) {
        throw "Visual Studio 2022 with MSBuild support was not found."
    }
    return $path
}

function Enter-VsBuildEnvironment {
    if ($script:VsEnvLoaded) {
        return
    }

    $vsPath = Get-VSInstallPath
    $vcVars = Join-Path $vsPath "VC\Auxiliary\Build\vcvarsall.bat"
    if (-not (Test-Path -LiteralPath $vcVars)) {
        throw "vcvarsall.bat was not found under $vsPath"
    }

    Write-Info "Loading Visual Studio build environment from $vsPath"
    Push-Location (Split-Path -Parent $vcVars)
    try {
        cmd /c "`"$vcVars`" x64 >nul && set" | ForEach-Object {
            if ($_ -match "=") {
                $parts = $_ -split "=", 2
                [Environment]::SetEnvironmentVariable($parts[0], $parts[1], "Process")
            }
        }
    }
    finally {
        Pop-Location
    }

    $script:VsEnvLoaded = $true
    Write-Ok "Visual Studio environment ready."
}

function Invoke-LoggedCommand {
    param(
        [Parameter(Mandatory)] [string]$Label,
        [Parameter(Mandatory)] [string]$FilePath,
        [string[]]$Arguments = @(),
        [string]$WorkingDirectory = $PWD.Path,
        [switch]$AllowFailure
    )

    if ($VerboseOutput) {
        Write-Detail ("[{0}] {1} {2}" -f $Label, $FilePath, ($Arguments -join " "))
    }
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    $output = @()
    $exitCode = 0

    if ($VerboseOutput) {
        Push-Location $WorkingDirectory
        try {
            $previousErrorActionPreference = $ErrorActionPreference
            $hadNativeCommandPreference = Test-Path Variable:\PSNativeCommandUseErrorActionPreference
            if ($hadNativeCommandPreference) {
                $previousNativeCommandPreference = $PSNativeCommandUseErrorActionPreference
            }

            try {
                # Native tools often write progress and diagnostics to stderr even when
                # PowerShell should decide failure from the process exit code.
                $ErrorActionPreference = "Continue"
                if ($hadNativeCommandPreference) {
                    $PSNativeCommandUseErrorActionPreference = $false
                }

                $output = & $FilePath @Arguments 2>&1
                $exitCode = $LASTEXITCODE
            }
            finally {
                $ErrorActionPreference = $previousErrorActionPreference
                if ($hadNativeCommandPreference) {
                    $PSNativeCommandUseErrorActionPreference = $previousNativeCommandPreference
                }
            }
        }
        finally {
            Pop-Location
        }
    }
    else {
        $resolvedCommand = $FilePath
        $commandInfo = Get-Command $FilePath -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($commandInfo -and $commandInfo.Source) {
            $resolvedCommand = $commandInfo.Source
        }

        $stdoutPath = Join-Path $script:WorkRoot ("cmd-" + [guid]::NewGuid().ToString("N") + ".stdout.log")
        $stderrPath = Join-Path $script:WorkRoot ("cmd-" + [guid]::NewGuid().ToString("N") + ".stderr.log")
        try {
            $startProcessParameters = @{
                FilePath = $resolvedCommand
                WorkingDirectory = $WorkingDirectory
                NoNewWindow = $true
                RedirectStandardOutput = $stdoutPath
                RedirectStandardError = $stderrPath
                PassThru = $true
            }
            if ($Arguments.Count -gt 0) {
                $startProcessParameters["ArgumentList"] = $Arguments
            }

            $process = Start-Process @startProcessParameters
            $script:ActiveProcesses.Add($process)

            $lastStatusLine = $null
            $lastHeartbeatBucket = -1
            $lastSeenLineCount = 0
            Start-StatusPanel -Title ("{0} (starting)" -f $Label)
            Push-StatusPanelLine ("working directory: {0}" -f $WorkingDirectory)
            while (-not $process.HasExited) {
                $recentLines = @(Get-RecentProcessLines -Paths @($stderrPath, $stdoutPath) -TailCount 50)
                $elapsedSeconds = $sw.Elapsed.TotalSeconds
                Update-StatusPanelTitle -Title ("{0} ({1:n1}s)" -f $Label, $elapsedSeconds)

                $newLineCount = $recentLines.Count
                if ($newLineCount -gt $lastSeenLineCount) {
                    # Push only lines we haven't shown yet
                    $startIdx = [Math]::Max(0, $newLineCount - ($newLineCount - $lastSeenLineCount))
                    for ($li = $lastSeenLineCount; $li -lt $newLineCount; $li++) {
                        # Clamp index to valid range (tail may have shifted)
                        if ($li -lt $recentLines.Count) {
                            Push-StatusPanelLine -Message $recentLines[$li]
                        }
                    }
                    $lastSeenLineCount = $newLineCount
                    $lastStatusLine = $recentLines[-1]
                }
                elseif ($recentLines.Count -gt 0 -and $recentLines[-1] -ne $lastStatusLine) {
                    # Log file was recycled or shifted — refresh the whole panel
                    $script:StatusPanelLines = [System.Collections.ArrayList]::new()
                    foreach ($rl in $recentLines) {
                        Push-StatusPanelLine -Message $rl
                    }
                    $lastSeenLineCount = $recentLines.Count
                    $lastStatusLine = $recentLines[-1]
                }
                else {
                    $heartbeatBucket = [int][Math]::Floor($elapsedSeconds / 5)
                    if ($heartbeatBucket -gt $lastHeartbeatBucket) {
                        $heartbeatMsg = "still running ({0:n1}s)" -f $elapsedSeconds
                        if ($lastStatusLine) {
                            $heartbeatMsg = "{0} => {1}" -f $heartbeatMsg, $lastStatusLine
                        }
                        Push-StatusPanelLine -Message $heartbeatMsg
                        $lastHeartbeatBucket = $heartbeatBucket
                    }
                }

                Start-Sleep -Milliseconds 250
            }

            $process.WaitForExit()
            $process.Refresh()
            try {
                $exitCode = [int]$process.ExitCode
            }
            catch {
                $exitCode = -1
            }
            $output = @(
                @(if (Test-Path -LiteralPath $stdoutPath -PathType Leaf) { Get-Content -LiteralPath $stdoutPath })
                @(if (Test-Path -LiteralPath $stderrPath -PathType Leaf) { Get-Content -LiteralPath $stderrPath })
            )
        }
        finally {
            Clear-StatusLine
            if ($process) {
                try {
                    if (-not $process.HasExited) {
                        & taskkill /T /F /PID $process.Id 2>$null | Out-Null
                    }
                } catch {}
                $script:ActiveProcesses.Remove($process) | Out-Null
                $process.Dispose()
            }
            foreach ($path in @($stdoutPath, $stderrPath)) {
                if ($path -and (Test-Path -LiteralPath $path -PathType Leaf)) {
                    Remove-FileWithRetry -Path $path
                }
            }
        }
    }

    if ($VerboseOutput) {
        foreach ($line in $output) {
            if ($line -is [System.Management.Automation.ErrorRecord]) {
                Write-Host ("    {0}" -f $line.Exception.Message) -ForegroundColor Red
            }
            else {
                $text = [string]$line
                if ($text -match "error|fatal") {
                    Write-Host ("    {0}" -f $text) -ForegroundColor Red
                }
                elseif ($text -match "warning") {
                    Write-Host ("    {0}" -f $text) -ForegroundColor Yellow
                }
                elseif ($text -match "^\s*(\[\s*\d+%\]|-- )") {
                    Write-Host ("    {0}" -f $text) -ForegroundColor Green
                }
                else {
                    Write-Host ("    {0}" -f $text)
                }
            }
        }
    }

    $sw.Stop()
    if ($null -eq $exitCode -or [string]::IsNullOrWhiteSpace([string]$exitCode)) {
        $exitCode = -1
    }
    if ($exitCode -ne 0 -and -not $AllowFailure) {
        $tail = @($output | Select-Object -Last 40)
        foreach ($line in $tail) {
            Write-Host ("    {0}" -f $line) -ForegroundColor DarkGray
        }
        throw ("Command failed: {0} (exit code {1})" -f $Label, $exitCode)
    }

    if ($exitCode -eq 0) {
        Write-Ok ("{0} ({1:n1}s)" -f $Label, $sw.Elapsed.TotalSeconds)
    }

    return [pscustomobject]@{
        ExitCode = $exitCode
        Output = $output
    }
}

function Copy-DirectoryContent {
    param(
        [Parameter(Mandatory)] [string]$Source,
        [Parameter(Mandatory)] [string]$Destination
    )

    if (-not (Test-Path -LiteralPath $Source -PathType Container)) {
        return
    }

    Ensure-Directory $Destination
    Copy-Item -Path (Join-Path $Source "*") -Destination $Destination -Recurse -Force
}

function Copy-FirstMatch {
    param(
        [Parameter(Mandatory)] [string]$SearchRoot,
        [Parameter(Mandatory)] [string[]]$Patterns,
        [Parameter(Mandatory)] [string]$Destination,
        [switch]$AllowMany
    )

    Ensure-Directory $Destination
    $matches = @()
    foreach ($pattern in $Patterns) {
        $matches += Get-ChildItem -Path $SearchRoot -Recurse -File -Filter $pattern -ErrorAction SilentlyContinue
    }

    $matches = @($matches | Sort-Object FullName -Unique)
    if (-not $matches) {
        return $false
    }

    if (-not $AllowMany) {
        $matches = @($matches[0])
    }

    foreach ($match in $matches) {
        Copy-Item -LiteralPath $match.FullName -Destination (Join-Path $Destination $match.Name) -Force
    }

    return $true
}

function Download-File {
    param(
        [Parameter(Mandatory)] [string[]]$Sources,
        [Parameter(Mandatory)] [string]$Destination
    )

    if (Test-Path -LiteralPath $Destination -PathType Leaf) {
        Write-Ok ("Archive already downloaded: {0}" -f ([System.IO.Path]::GetFileName($Destination)))
        return
    }

    Ensure-Directory (Split-Path -Parent $Destination)
    $errors = @()
    foreach ($source in $Sources) {
        Write-Info "Downloading $source"

        try {
            if ($source -notmatch '^[a-zA-Z][a-zA-Z0-9+.-]*://') {
                if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
                    throw "Local source file was not found."
                }

                Copy-Item -LiteralPath $source -Destination $Destination -Force
                Write-Ok ("Saved archive to {0}" -f $Destination)
                return
            }

            $downloadedViaBits = $false
            if (Get-Module -ListAvailable -Name BitsTransfer) {
                try {
                    Import-Module BitsTransfer
                    Start-BitsTransfer -Source $source -Destination $Destination -DisplayName ([System.IO.Path]::GetFileName($Destination)) -ErrorAction Stop
                    $downloadedViaBits = $true
                }
                catch {
                    Write-Warn ("BITS download failed, falling back to Invoke-WebRequest: {0}" -f $_.Exception.Message)
                    if (Test-Path -LiteralPath $Destination -PathType Leaf) {
                        Remove-Item -LiteralPath $Destination -Force
                    }
                }
            }

            if (-not $downloadedViaBits) {
                $invokeWebRequestParameters = @{
                    Uri = $source
                    OutFile = $Destination
                    ErrorAction = "Stop"
                }
                if ((Get-Command Invoke-WebRequest).Parameters.ContainsKey("UseBasicParsing")) {
                    $invokeWebRequestParameters["UseBasicParsing"] = $true
                }

                $previousProgressPreference = $ProgressPreference
                try {
                    $ProgressPreference = "SilentlyContinue"
                    Invoke-WebRequest @invokeWebRequestParameters
                }
                finally {
                    $ProgressPreference = $previousProgressPreference
                }
            }

            Write-Ok ("Saved archive to {0}" -f $Destination)
            return
        }
        catch {
            if (Test-Path -LiteralPath $Destination -PathType Leaf) {
                Remove-Item -LiteralPath $Destination -Force -ErrorAction SilentlyContinue
            }
            $message = $_.Exception.Message
            if ($source -like "https://dev.mysql.com/*" -and $message -match "\(403\)\s+Forbidden") {
                $message = "{0}. Oracle blocked the automated download. Add a fallback local archive path in {1} using ';' separators." -f $message, $script:DepManifestPath
            }
            $errors += ("{0} -> {1}" -f $source, $message)
            Write-Warn ("Download source failed: {0}" -f $source)
        }
    }

    throw ("All download sources failed for {0}: {1}" -f ([System.IO.Path]::GetFileName($Destination)), ($errors -join " | "))
}

function Expand-ArchiveSmart {
    param(
        [Parameter(Mandatory)] [string]$ArchivePath,
        [Parameter(Mandatory)] [string]$Destination
    )

    if (Test-Path -LiteralPath $Destination -PathType Container) {
        Write-Ok ("Source already extracted: {0}" -f $Destination)
        return
    }

    $tempDir = $Destination + ".extracting"
    Reset-Directory $tempDir
    $lower = $ArchivePath.ToLowerInvariant()

    if ($lower.EndsWith(".zip")) {
        Expand-Archive -LiteralPath $ArchivePath -DestinationPath $tempDir -Force
    }
    else {
        Invoke-LoggedCommand -Label "extract $([System.IO.Path]::GetFileName($ArchivePath))" -FilePath "tar" -Arguments @("-xf", $ArchivePath, "-C", $tempDir) -WorkingDirectory $script:ProjectRoot | Out-Null
    }

    $children = @(Get-ChildItem -LiteralPath $tempDir -Force)
    if ($children.Count -eq 1 -and $children[0].PSIsContainer) {
        Move-Item -LiteralPath $children[0].FullName -Destination $Destination
        Remove-Item -LiteralPath $tempDir -Recurse -Force
    }
    else {
        Move-Item -LiteralPath $tempDir -Destination $Destination
    }

    Write-Ok ("Extracted to {0}" -f $Destination)
}

function Get-SourceTree {
    param([pscustomobject]$Dependency)

    $archivePath = Get-ArchivePath $Dependency
    $extractPath = Get-ExtractPath $Dependency

    if (-not $BuildOnly) {
        $sources = if ($Dependency.PSObject.Properties["Urls"] -and $Dependency.Urls.Count -gt 0) { $Dependency.Urls } else { @($Dependency.Url) }
        Download-File -Sources $sources -Destination $archivePath
    }
    elseif (-not (Test-Path -LiteralPath $archivePath -PathType Leaf)) {
        throw "--BuildOnly was requested but archive is missing: $archivePath"
    }

    if (-not $DownloadOnly) {
        Expand-ArchiveSmart -ArchivePath $archivePath -Destination $extractPath
    }

    return $extractPath
}

function New-DualBuildLayout {
    param([Parameter(Mandatory)] [string]$Name)

    $layout = [pscustomobject]@{
        BuildRelease = Join-Path $script:BuildRoot "$Name\release"
        BuildDebug = Join-Path $script:BuildRoot "$Name\debug"
        StageRelease = Join-Path $script:BuildRoot "$Name\stage-release"
        StageDebug = Join-Path $script:BuildRoot "$Name\stage-debug"
    }

    foreach ($path in @($layout.BuildRelease, $layout.BuildDebug, $layout.StageRelease, $layout.StageDebug)) {
        if ($Clean -and (Test-Path -LiteralPath $path -PathType Container)) {
            Remove-Item -LiteralPath $path -Recurse -Force
        }
        Ensure-Directory $path
    }

    return $layout
}

function Invoke-CMakeInstallPair {
    param(
        [Parameter(Mandatory)] [string]$Label,
        [Parameter(Mandatory)] [string]$SourcePath,
        [Parameter(Mandatory)] $Layout,
        [Parameter(Mandatory)] [string[]]$CommonArguments,
        [string]$ReleaseConfig = "RelWithDebInfo",
        [string]$DebugConfig = "Debug"
    )

    $configureReleaseArgs = @(
        "-S", $SourcePath,
        "-B", $Layout.BuildRelease,
        "-G", $Generator,
        "-A", "x64",
        "-DCMAKE_INSTALL_PREFIX=$($Layout.StageRelease)"
    ) + $CommonArguments
    Invoke-LoggedCommand -Label "$Label configure release" -FilePath "cmake" -Arguments $configureReleaseArgs -WorkingDirectory $script:ProjectRoot | Out-Null

    Invoke-LoggedCommand -Label "$Label build release" -FilePath "cmake" -Arguments @(
        "--build", $Layout.BuildRelease,
        "--target", "INSTALL",
        "--config", $ReleaseConfig,
        "--parallel", $Jobs
    ) -WorkingDirectory $script:ProjectRoot | Out-Null

    $configureDebugArgs = @(
        "-S", $SourcePath,
        "-B", $Layout.BuildDebug,
        "-G", $Generator,
        "-A", "x64",
        "-DCMAKE_INSTALL_PREFIX=$($Layout.StageDebug)"
    ) + $CommonArguments
    Invoke-LoggedCommand -Label "$Label configure debug" -FilePath "cmake" -Arguments $configureDebugArgs -WorkingDirectory $script:ProjectRoot | Out-Null

    Invoke-LoggedCommand -Label "$Label build debug" -FilePath "cmake" -Arguments @(
        "--build", $Layout.BuildDebug,
        "--target", "INSTALL",
        "--config", $DebugConfig,
        "--parallel", $Jobs
    ) -WorkingDirectory $script:ProjectRoot | Out-Null
}

function Copy-MatchingFiles {
    param(
        [Parameter(Mandatory)] [string[]]$SearchRoots,
        [Parameter(Mandatory)] [string[]]$Patterns,
        [Parameter(Mandatory)] [string]$Destination
    )

    Ensure-Directory $Destination
    foreach ($root in $SearchRoots) {
        if (-not (Test-Path -LiteralPath $root -PathType Container)) {
            continue
        }

        foreach ($pattern in $Patterns) {
            $items = Get-ChildItem -Path $root -Recurse -File -Filter $pattern -ErrorAction SilentlyContinue
            foreach ($item in $items) {
                Copy-Item -LiteralPath $item.FullName -Destination (Join-Path $Destination $item.Name) -Force
            }
        }
    }
}

function Build-OpenSsl {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $layout = New-DualBuildLayout "openssl"
    $srcRelease = Join-Path $script:BuildRoot "openssl\src-release"
    $srcDebug = Join-Path $script:BuildRoot "openssl\src-debug"
    foreach ($path in @($srcRelease, $srcDebug)) {
        if ($Clean -and (Test-Path -LiteralPath $path -PathType Container)) {
            Remove-Item -LiteralPath $path -Recurse -Force
        }
        if (-not (Test-Path -LiteralPath $path -PathType Container)) {
            Ensure-Directory $path
            Copy-Item -Path (Join-Path $sourcePath "*") -Destination $path -Recurse -Force
        }
    }

    Invoke-LoggedCommand -Label "openssl configure release" -FilePath "perl" -WorkingDirectory $srcRelease -Arguments @(
        "Configure", "VC-WIN64A", "shared", "no-tests",
        "--prefix=$($layout.StageRelease)",
        "--openssldir=$($layout.StageRelease)"
    ) | Out-Null
    Invoke-LoggedCommand -Label "openssl build release" -FilePath "nmake" -WorkingDirectory $srcRelease | Out-Null
    Invoke-LoggedCommand -Label "openssl install release" -FilePath "nmake" -WorkingDirectory $srcRelease -Arguments @("install_sw") | Out-Null

    Invoke-LoggedCommand -Label "openssl configure debug" -FilePath "perl" -WorkingDirectory $srcDebug -Arguments @(
        "Configure", "debug-VC-WIN64A", "shared", "no-tests",
        "--prefix=$($layout.StageDebug)",
        "--openssldir=$($layout.StageDebug)"
    ) | Out-Null
    Invoke-LoggedCommand -Label "openssl build debug" -FilePath "nmake" -WorkingDirectory $srcDebug | Out-Null
    Invoke-LoggedCommand -Label "openssl install debug" -FilePath "nmake" -WorkingDirectory $srcDebug -Arguments @("install_sw") | Out-Null

    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "include") -Destination (Join-Path $script:BundleDir "include")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("*.dll", "*.lib", "*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("*.dll", "*.lib", "*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")

    Write-Ok "OpenSSL release and debug artefacts staged."
}

function Build-ZlibSource {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $layout = New-DualBuildLayout "zlib"
    $commonArgs = @("-DBUILD_SHARED_LIBS=ON")
    Invoke-CMakeInstallPair -Label "zlib" -SourcePath $sourcePath -Layout $layout -CommonArguments $commonArgs -ReleaseConfig "Release" -DebugConfig "Debug"

    Ensure-Directory (Join-Path $script:BundleDir "include\zlib")
    Copy-MatchingFiles -SearchRoots @($sourcePath) -Patterns @("*.h") -Destination (Join-Path $script:BundleDir "include\zlib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("zlib*.dll", "zlib*.lib", "zlib*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("zlib*.dll", "zlib*.lib", "zlib*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")

    Write-Ok "zlib release and debug artefacts staged."
}

function Build-LibXml2 {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $layout = New-DualBuildLayout "libxml2"
    $commonArgs = @(
        "-DLIBXML2_WITH_ICONV=OFF",
        "-DLIBXML2_WITH_PYTHON=OFF",
        "-DLIBXML2_WITH_TESTS=OFF",
        "-DBUILD_SHARED_LIBS=ON"
    )
    Invoke-CMakeInstallPair -Label "libxml2" -SourcePath $sourcePath -Layout $layout -CommonArguments $commonArgs -ReleaseConfig "Release" -DebugConfig "Debug"

    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "include") -Destination (Join-Path $script:BundleDir "include")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("libxml2*.dll", "libxml2*.lib", "libxml2*.pdb", "iconv*.dll", "iconv*.lib", "iconv*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("libxml2*.dll", "libxml2*.lib", "libxml2*.pdb", "iconv*.dll", "iconv*.lib", "iconv*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")

    Write-Ok "libxml2 release and debug artefacts staged."
}

function Build-CairoBundle {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $layout = New-DualBuildLayout "cairo"

    Invoke-LoggedCommand -Label "cairo meson setup release" -FilePath "meson" -Arguments @(
        "setup", $layout.BuildRelease, $sourcePath,
        "--prefix", $layout.StageRelease,
        "--buildtype", "release",
        "--default-library", "shared",
        "--wipe"
    ) -WorkingDirectory $script:ProjectRoot | Out-Null
    Invoke-LoggedCommand -Label "cairo ninja release" -FilePath "ninja" -Arguments @("-C", $layout.BuildRelease) -WorkingDirectory $script:ProjectRoot | Out-Null
    Invoke-LoggedCommand -Label "cairo install release" -FilePath "ninja" -Arguments @("-C", $layout.BuildRelease, "install") -WorkingDirectory $script:ProjectRoot | Out-Null

    Invoke-LoggedCommand -Label "cairo meson setup debug" -FilePath "meson" -Arguments @(
        "setup", $layout.BuildDebug, $sourcePath,
        "--prefix", $layout.StageDebug,
        "--buildtype", "debug",
        "--default-library", "shared",
        "--wipe"
    ) -WorkingDirectory $script:ProjectRoot | Out-Null
    Invoke-LoggedCommand -Label "cairo ninja debug" -FilePath "ninja" -Arguments @("-C", $layout.BuildDebug) -WorkingDirectory $script:ProjectRoot | Out-Null
    Invoke-LoggedCommand -Label "cairo install debug" -FilePath "ninja" -Arguments @("-C", $layout.BuildDebug, "install") -WorkingDirectory $script:ProjectRoot | Out-Null

    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "include") -Destination (Join-Path $script:BundleDir "include")
    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "lib\glib-2.0\include") -Destination (Join-Path $script:BundleDir "lib\glib-2.0\include")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("*.dll", "*.lib", "*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("*.dll", "*.lib", "*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")

    Write-Ok "cairo/glib bundle staged."
}

function Build-LibZip {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $layout = New-DualBuildLayout "libzip"
    $commonArgs = @(
        "-DBUILD_SHARED_LIBS=ON",
        "-DBUILD_TOOLS=OFF",
        "-DBUILD_REGRESS=OFF",
        "-DBUILD_EXAMPLES=OFF",
        "-DENABLE_BZIP2=OFF",
        "-DENABLE_LZMA=OFF",
        "-DENABLE_ZSTD=OFF",
        "-DENABLE_COMMONCRYPTO=OFF",
        "-DENABLE_GNUTLS=OFF",
        "-DENABLE_MBEDTLS=OFF",
        "-DENABLE_OPENSSL=OFF",
        "-DZLIB_INCLUDE_DIR=$([System.IO.Path]::Combine($script:BundleDir, 'include', 'zlib'))",
        "-DZLIB_LIBRARY=$([System.IO.Path]::Combine($script:BundleDir, 'lib', 'zlib.lib'))",
        "-DZLIB_LIBRARY_RELEASE=$([System.IO.Path]::Combine($script:BundleDir, 'lib', 'zlib.lib'))",
        "-DZLIB_LIBRARY_DEBUG=$([System.IO.Path]::Combine($script:BundleDir, 'debug', 'lib', 'zlibd.lib'))"
    )
    Invoke-CMakeInstallPair -Label "libzip" -SourcePath $sourcePath -Layout $layout -CommonArguments $commonArgs -ReleaseConfig "Release" -DebugConfig "Debug"

    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "include") -Destination (Join-Path $script:BundleDir "include")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("zip*.dll", "zip*.lib", "zip*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("zip*.dll", "zip*.lib", "zip*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")

    Write-Ok "libzip release and debug artefacts staged."
}

function Build-AntlrRuntime {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $cmakeRoot = $sourcePath
    if (-not (Test-Path -LiteralPath (Join-Path $cmakeRoot "CMakeLists.txt") -PathType Leaf) -and
        (Test-Path -LiteralPath (Join-Path $cmakeRoot "runtime\Cpp\CMakeLists.txt") -PathType Leaf)) {
        $cmakeRoot = Join-Path $cmakeRoot "runtime\Cpp"
    }

    $layout = New-DualBuildLayout "antlr4"
    $commonArgs = @(
        "-DBUILD_SHARED_LIBS=ON",
        "-DANTLR4_INSTALL=ON",
        "-DWITH_DEMO=OFF"
    )
    Invoke-CMakeInstallPair -Label "antlr4-runtime" -SourcePath $cmakeRoot -Layout $layout -CommonArguments $commonArgs -ReleaseConfig "Release" -DebugConfig "Debug"

    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "include") -Destination (Join-Path $script:BundleDir "include")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("antlr4*.dll", "antlr4*.lib", "antlr4*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("antlr4*.dll", "antlr4*.lib", "antlr4*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")

    Write-Ok "ANTLR4 runtime release and debug artefacts staged."
}

function Build-LibSsh {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $layout = New-DualBuildLayout "libssh"
    $commonArgs = @(
        "-DBUILD_SHARED_LIBS=ON",
        "-DWITH_EXAMPLES=OFF",
        "-DWITH_SERVER=OFF",
        "-DWITH_GSSAPI=OFF",
        "-DUNIT_TESTING=OFF",
        "-DCMAKE_PREFIX_PATH=$script:BundleDir",
        "-DOPENSSL_ROOT_DIR=$script:BundleDir",
        "-DZLIB_INCLUDE_DIR=$([System.IO.Path]::Combine($script:BundleDir, 'include', 'zlib'))",
        "-DZLIB_LIBRARY_RELEASE=$([System.IO.Path]::Combine($script:BundleDir, 'lib', 'zlib.lib'))",
        "-DZLIB_LIBRARY_DEBUG=$([System.IO.Path]::Combine($script:BundleDir, 'debug', 'lib', 'zlibd.lib'))"
    )
    Invoke-CMakeInstallPair -Label "libssh" -SourcePath $sourcePath -Layout $layout -CommonArguments $commonArgs -ReleaseConfig "Release" -DebugConfig "Debug"

    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "include") -Destination (Join-Path $script:BundleDir "include")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("ssh*.dll", "ssh*.lib", "ssh*.pdb", "libssh*.dll", "libssh*.lib", "libssh*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("ssh*.dll", "ssh*.lib", "ssh*.pdb", "libssh*.dll", "libssh*.lib", "libssh*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")

    Write-Ok "libssh release and debug artefacts staged."
}

function Build-Proj {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $layout = New-DualBuildLayout "proj"
    $commonArgs = @(
        "-DBUILD_SHARED_LIBS=ON",
        "-DBUILD_TESTING=OFF",
        "-DBUILD_CCT=OFF",
        "-DBUILD_CS2CS=OFF",
        "-DBUILD_GEOD=OFF",
        "-DBUILD_GIE=OFF",
        "-DENABLE_CURL=OFF",
        "-DCMAKE_PREFIX_PATH=$script:BundleDir",
        "-DSQLite3_INCLUDE_DIR=$([System.IO.Path]::Combine($script:BundleDir, 'include', 'sqlite'))",
        "-DSQLite3_LIBRARY=$([System.IO.Path]::Combine($script:BundleDir, 'lib', 'sqlite3.lib'))",
        "-DSQLite3_LIBRARY_RELEASE=$([System.IO.Path]::Combine($script:BundleDir, 'lib', 'sqlite3.lib'))",
        "-DSQLite3_LIBRARY_DEBUG=$([System.IO.Path]::Combine($script:BundleDir, 'debug', 'lib', 'sqlite3_d.lib'))"
    )
    Invoke-CMakeInstallPair -Label "proj" -SourcePath $sourcePath -Layout $layout -CommonArguments $commonArgs -ReleaseConfig "Release" -DebugConfig "Debug"

    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "include") -Destination (Join-Path $script:BundleDir "include")
    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "share\proj") -Destination (Join-Path $script:BundleDir "share\proj")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("proj*.dll", "proj*.lib", "proj*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("proj*.dll", "proj*.lib", "proj*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")

    Write-Ok "PROJ release and debug artefacts staged."
}

function Build-Gdal {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $layout = New-DualBuildLayout "gdal"
    $commonArgs = @(
        "-DBUILD_SHARED_LIBS=ON",
        "-DBUILD_APPS=ON",
        "-DBUILD_TESTING=OFF",
        "-DGDAL_BUILD_OPTIONAL_DRIVERS=OFF",
        "-DOGR_BUILD_OPTIONAL_DRIVERS=OFF",
        "-DGDAL_USE_CURL=OFF",
        "-DGDAL_USE_SQLITE3=OFF",
        "-DGDAL_USE_LIBXML2=ON",
        "-DGDAL_USE_ZLIB=ON",
        "-DGDAL_USE_PNG=OFF",
        "-DGDAL_USE_JPEG=OFF",
        "-DGDAL_USE_TIFF=OFF",
        "-DGDAL_USE_GEOS=OFF",
        "-DGDAL_USE_ICONV=OFF",
        "-DGDAL_USE_EXPAT=OFF",
        "-DGDAL_USE_PROJ=ON",
        "-DCMAKE_PREFIX_PATH=$script:BundleDir",
        "-DZLIB_INCLUDE_DIR=$([System.IO.Path]::Combine($script:BundleDir, 'include', 'zlib'))",
        "-DZLIB_LIBRARY=$([System.IO.Path]::Combine($script:BundleDir, 'lib', 'zlib.lib'))",
        "-DLIBXML2_INCLUDE_DIR=$([System.IO.Path]::Combine($script:BundleDir, 'include', 'libxml2'))",
        "-DLIBXML2_LIBRARY=$([System.IO.Path]::Combine($script:BundleDir, 'lib', 'libxml2.lib'))",
        "-DPROJ_INCLUDE_DIR=$([System.IO.Path]::Combine($script:BundleDir, 'include'))",
        "-DPROJ_LIBRARY=$([System.IO.Path]::Combine($script:BundleDir, 'lib', 'proj.lib'))"
    )
    Invoke-CMakeInstallPair -Label "gdal" -SourcePath $sourcePath -Layout $layout -CommonArguments $commonArgs -ReleaseConfig "Release" -DebugConfig "Debug"

    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "include") -Destination (Join-Path $script:BundleDir "include")
    Copy-DirectoryContent -Source (Join-Path $layout.StageRelease "share\gdal") -Destination (Join-Path $script:BundleDir "share\gdal")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin"), (Join-Path $layout.StageRelease "lib")) -Patterns @("gdal*.dll", "gdal*.lib", "gdal*.pdb") -Destination (Join-Path $script:BundleDir "lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageDebug "bin"), (Join-Path $layout.StageDebug "lib")) -Patterns @("gdal*.dll", "gdal*.lib", "gdal*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib")
    Copy-MatchingFiles -SearchRoots @((Join-Path $layout.StageRelease "bin")) -Patterns @("ogrinfo.exe", "ogr2ogr.exe") -Destination (Join-Path $script:BundleDir "bin")

    Write-Ok "GDAL release and debug artefacts staged."
}

function Build-BoostHeaders {
    param([pscustomobject]$Dependency)

    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $dest = Join-Path $script:BundleDir "include\boost"
    Reset-Directory $dest

    $headerRoots = @()
    $topLevelBoostDir = Join-Path $sourcePath "boost"
    if (Test-Path -LiteralPath $topLevelBoostDir -PathType Container) {
        $headerRoots += $topLevelBoostDir
    }
    else {
        $headerRoots = @(
            Get-ChildItem -LiteralPath (Join-Path $sourcePath "libs") -Directory -ErrorAction SilentlyContinue |
                ForEach-Object { Join-Path $_.FullName "include\boost" } |
                Where-Object { Test-Path -LiteralPath $_ -PathType Container }
        )
    }

    if (-not $headerRoots -or $headerRoots.Count -eq 0) {
        throw "Boost headers were not found under $sourcePath"
    }

    foreach ($headerRoot in $headerRoots) {
        Copy-DirectoryContent -Source $headerRoot -Destination $dest
    }

    Write-Ok "Boost headers staged."
}

function Build-RapidJsonHeaders {
    param([pscustomobject]$Dependency)

    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $rapidDir = Join-Path $sourcePath "include\rapidjson"
    if (-not (Test-Path -LiteralPath $rapidDir -PathType Container)) {
        throw "RapidJSON headers were not found under $rapidDir"
    }

    $dest = Join-Path $script:BundleDir "include\rapidjson"
    Reset-Directory $dest
    Copy-DirectoryContent -Source $rapidDir -Destination $dest
    Write-Ok "RapidJSON headers staged."
}

function Build-Sqlite {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $buildRel = Join-Path $script:BuildRoot "sqlite\release"
    $buildDbg = Join-Path $script:BuildRoot "sqlite\debug"
    Ensure-Directory $buildRel
    Ensure-Directory $buildDbg

    $sqliteC = Join-Path $sourcePath "sqlite3.c"
    $sqliteH = Join-Path $sourcePath "sqlite3.h"
    $sqliteExtH = Join-Path $sourcePath "sqlite3ext.h"
    if (-not (Test-Path -LiteralPath $sqliteC -PathType Leaf)) {
        throw "sqlite3.c was not found in $sourcePath"
    }

    $commonDefines = @(
        "/DSQLITE_ENABLE_COLUMN_METADATA",
        "/DSQLITE_ENABLE_FTS5",
        "/DSQLITE_ENABLE_JSON1",
        "/DSQLITE_ENABLE_RTREE",
        "/DSQLITE_ENABLE_UNLOCK_NOTIFY",
        "/DSQLITE_THREADSAFE=1",
        "/DSQLITE_API=__declspec(dllexport)"
    )

    $sqliteReleaseArgs = @(
        "/nologo", "/O2", "/MD", "/LD", "/Zi"
    ) + $commonDefines + @(
        $sqliteC,
        "/link", "/NOLOGO",
        "/OUT:sqlite3.dll",
        "/IMPLIB:sqlite3.lib",
        "/PDB:sqlite3.pdb"
    )
    Invoke-LoggedCommand -Label "sqlite release build" -FilePath "cl.exe" -WorkingDirectory $buildRel -Arguments $sqliteReleaseArgs | Out-Null

    $sqliteDebugArgs = @(
        "/nologo", "/Od", "/MDd", "/LD", "/Zi", "/DSQLITE_DEBUG"
    ) + $commonDefines + @(
        $sqliteC,
        "/link", "/NOLOGO",
        "/OUT:sqlite3_d.dll",
        "/IMPLIB:sqlite3_d.lib",
        "/PDB:sqlite3_d.pdb"
    )
    Invoke-LoggedCommand -Label "sqlite debug build" -FilePath "cl.exe" -WorkingDirectory $buildDbg -Arguments $sqliteDebugArgs | Out-Null

    Ensure-Directory (Join-Path $script:BundleDir "include\sqlite")
    Copy-Item -LiteralPath $sqliteH -Destination (Join-Path $script:BundleDir "include\sqlite\sqlite3.h") -Force
    if (Test-Path -LiteralPath $sqliteExtH -PathType Leaf) {
        Copy-Item -LiteralPath $sqliteExtH -Destination (Join-Path $script:BundleDir "include\sqlite\sqlite3ext.h") -Force
    }

    foreach ($item in @("sqlite3.dll", "sqlite3.lib", "sqlite3.pdb")) {
        Copy-Item -LiteralPath (Join-Path $buildRel $item) -Destination (Join-Path $script:BundleDir "lib\$item") -Force
    }
    foreach ($item in @("sqlite3_d.dll", "sqlite3_d.lib", "sqlite3_d.pdb")) {
        Copy-Item -LiteralPath (Join-Path $buildDbg $item) -Destination (Join-Path $script:BundleDir "debug\lib\$item") -Force
    }

    Write-Ok "SQLite release and debug artefacts staged."
}

function Build-Vsqlitepp {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $headerSourceRoot = Join-Path $sourcePath "include\sqlite"
    if (-not (Test-Path -LiteralPath $headerSourceRoot -PathType Container)) {
        throw "vsqlitepp headers were not found under $headerSourceRoot"
    }

    $sourceRoot = Join-Path $sourcePath "src\sqlite"
    $sourceCandidates = @(
        Get-ChildItem -LiteralPath $sourceRoot -File -ErrorAction SilentlyContinue |
            Where-Object { $_.Extension -in @(".cpp", ".cc", ".cxx") }
    )

    if (-not $sourceCandidates) {
        throw "No vsqlitepp implementation source files were found under $sourceRoot"
    }

    $headerDest = Join-Path $script:BundleDir "include\sqlite"
    Reset-Directory $headerDest
    Copy-DirectoryContent -Source $headerSourceRoot -Destination $headerDest

    $buildRel = Join-Path $script:BuildRoot "vsqlitepp\release"
    $buildDbg = Join-Path $script:BuildRoot "vsqlitepp\debug"
    Reset-Directory $buildRel
    Reset-Directory $buildDbg

    $includeArgs = @(
        "/I" + (Join-Path $sourcePath "include"),
        "/I" + (Join-Path $script:BundleDir "include"),
        "/I" + (Join-Path $script:BundleDir "include\sqlite")
    )
    $sourceArgs = @($sourceCandidates | ForEach-Object { $_.FullName })

    $vsqliteReleaseSharedArgs = @(
        "/nologo", "/O2", "/MD", "/EHsc", "/LD", "/Zi"
    ) + $includeArgs + $sourceArgs + @(
        "sqlite3.lib",
        "/link", "/NOLOGO",
        "/LIBPATH:" + (Join-Path $script:BundleDir "lib"),
        "/OUT:vsqlite++.dll",
        "/IMPLIB:vsqlitepp.lib",
        "/PDB:vsqlitepp.pdb"
    )
    $releaseDll = Invoke-LoggedCommand -Label "vsqlitepp release shared build" -FilePath "cl.exe" -WorkingDirectory $buildRel -AllowFailure -Arguments $vsqliteReleaseSharedArgs
    $releaseSharedReady = (Test-Path -LiteralPath (Join-Path $buildRel "vsqlite++.dll") -PathType Leaf) -and
        (Test-Path -LiteralPath (Join-Path $buildRel "vsqlitepp.lib") -PathType Leaf)

    if ($releaseDll.ExitCode -ne 0 -or -not $releaseSharedReady) {
        Write-Warn "Shared build did not produce the expected vsqlitepp release artifacts, falling back to a static library."
        $vsqliteReleaseCompileArgs = @(
            "/nologo", "/O2", "/MD", "/EHsc", "/Zi", "/c"
        ) + $includeArgs + $sourceArgs
        Invoke-LoggedCommand -Label "vsqlitepp release compile" -FilePath "cl.exe" -WorkingDirectory $buildRel -Arguments $vsqliteReleaseCompileArgs | Out-Null
        $objects = @(Get-ChildItem -Path $buildRel -Filter *.obj | ForEach-Object { $_.FullName })
        $vsqliteReleaseLibArgs = @("/NOLOGO", "/OUT:vsqlitepp.lib") + $objects
        Invoke-LoggedCommand -Label "vsqlitepp release lib" -FilePath "lib.exe" -WorkingDirectory $buildRel -Arguments $vsqliteReleaseLibArgs | Out-Null
    }

    $vsqliteDebugSharedArgs = @(
        "/nologo", "/Od", "/MDd", "/EHsc", "/LD", "/Zi"
    ) + $includeArgs + $sourceArgs + @(
        "sqlite3_d.lib",
        "/link", "/NOLOGO",
        "/LIBPATH:" + (Join-Path $script:BundleDir "debug\lib"),
        "/OUT:vsqlite++.dll",
        "/IMPLIB:vsqlitepp.lib",
        "/PDB:vsqlitepp.pdb"
    )
    $debugDll = Invoke-LoggedCommand -Label "vsqlitepp debug shared build" -FilePath "cl.exe" -WorkingDirectory $buildDbg -AllowFailure -Arguments $vsqliteDebugSharedArgs
    $debugSharedReady = (Test-Path -LiteralPath (Join-Path $buildDbg "vsqlite++.dll") -PathType Leaf) -and
        (Test-Path -LiteralPath (Join-Path $buildDbg "vsqlitepp.lib") -PathType Leaf)

    if ($debugDll.ExitCode -ne 0 -or -not $debugSharedReady) {
        Write-Warn "Shared debug build did not produce the expected vsqlitepp artifacts, falling back to a static library."
        $vsqliteDebugCompileArgs = @(
            "/nologo", "/Od", "/MDd", "/EHsc", "/Zi", "/c"
        ) + $includeArgs + $sourceArgs
        Invoke-LoggedCommand -Label "vsqlitepp debug compile" -FilePath "cl.exe" -WorkingDirectory $buildDbg -Arguments $vsqliteDebugCompileArgs | Out-Null
        $objects = @(Get-ChildItem -Path $buildDbg -Filter *.obj | ForEach-Object { $_.FullName })
        $vsqliteDebugLibArgs = @("/NOLOGO", "/OUT:vsqlitepp.lib") + $objects
        Invoke-LoggedCommand -Label "vsqlitepp debug lib" -FilePath "lib.exe" -WorkingDirectory $buildDbg -Arguments $vsqliteDebugLibArgs | Out-Null
    }

    if (Test-Path -LiteralPath (Join-Path $buildRel "vsqlite++.dll")) {
        Copy-Item -LiteralPath (Join-Path $buildRel "vsqlite++.dll") -Destination (Join-Path $script:BundleDir "lib\vsqlite++.dll") -Force
        Copy-Item -LiteralPath (Join-Path $buildRel "vsqlitepp.lib") -Destination (Join-Path $script:BundleDir "lib\vsqlitepp.lib") -Force
        Copy-Item -LiteralPath (Join-Path $buildRel "vsqlitepp.pdb") -Destination (Join-Path $script:BundleDir "lib\vsqlitepp.pdb") -Force -ErrorAction SilentlyContinue
    }
    else {
        Copy-Item -LiteralPath (Join-Path $buildRel "vsqlitepp.lib") -Destination (Join-Path $script:BundleDir "lib\vsqlitepp.lib") -Force
    }

    if (Test-Path -LiteralPath (Join-Path $buildDbg "vsqlite++.dll")) {
        Copy-Item -LiteralPath (Join-Path $buildDbg "vsqlite++.dll") -Destination (Join-Path $script:BundleDir "debug\lib\vsqlite++.dll") -Force
        Copy-Item -LiteralPath (Join-Path $buildDbg "vsqlitepp.lib") -Destination (Join-Path $script:BundleDir "debug\lib\vsqlitepp.lib") -Force
        Copy-Item -LiteralPath (Join-Path $buildDbg "vsqlitepp.pdb") -Destination (Join-Path $script:BundleDir "debug\lib\vsqlitepp.pdb") -Force -ErrorAction SilentlyContinue
    }
    else {
        Copy-Item -LiteralPath (Join-Path $buildDbg "vsqlitepp.lib") -Destination (Join-Path $script:BundleDir "debug\lib\vsqlitepp.lib") -Force
    }

    Write-Ok "vsqlitepp headers and libraries staged."
}

function Build-Python {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $pcBuild = Join-Path $sourcePath "PCbuild"
    if (-not (Test-Path -LiteralPath $pcBuild -PathType Container)) {
        throw "Python PCbuild directory not found: $pcBuild"
    }

    Set-Content -LiteralPath (Join-Path $pcBuild "MSBuild.rsp") -Value "/p:PlatformToolset=v143"

    Invoke-LoggedCommand -Label "python release build" -FilePath (Join-Path $pcBuild "build.bat") -Arguments @("-c", "Release", "-p", "x64", "-t", "Build") -WorkingDirectory $pcBuild | Out-Null
    Invoke-LoggedCommand -Label "python debug build" -FilePath (Join-Path $pcBuild "build.bat") -Arguments @("-c", "Debug", "-p", "x64", "-t", "Build") -WorkingDirectory $pcBuild | Out-Null

    $amd64Dir = Join-Path $pcBuild "amd64"
    $pythonDest = Join-Path $script:BundleDir "python"
    Ensure-Directory $pythonDest
    Ensure-Directory (Join-Path $pythonDest "libs")
    Ensure-Directory (Join-Path $pythonDest "DLLs")
    Ensure-Directory (Join-Path $pythonDest "Include")

    Copy-Item -Path (Join-Path $amd64Dir "*.dll") -Destination $pythonDest -Force
    Copy-Item -Path (Join-Path $amd64Dir "*.exe") -Destination $pythonDest -Force
    Copy-Item -Path (Join-Path $amd64Dir "*.pdb") -Destination $pythonDest -Force -ErrorAction SilentlyContinue
    Copy-Item -Path (Join-Path $amd64Dir "*.lib") -Destination (Join-Path $pythonDest "libs") -Force
    Copy-Item -Path (Join-Path $amd64Dir "*.pyd") -Destination (Join-Path $pythonDest "DLLs") -Force

    Copy-DirectoryContent -Source (Join-Path $sourcePath "Lib") -Destination (Join-Path $pythonDest "Lib")
    Copy-DirectoryContent -Source (Join-Path $sourcePath "Tools") -Destination (Join-Path $pythonDest "Tools")
    Copy-DirectoryContent -Source (Join-Path $sourcePath "Include") -Destination (Join-Path $pythonDest "Include")

    $pyConfig = Join-Path $sourcePath "PC\pyconfig.h"
    if (Test-Path -LiteralPath $pyConfig -PathType Leaf) {
        Copy-Item -LiteralPath $pyConfig -Destination (Join-Path $pythonDest "Include\pyconfig.h") -Force
    }

    Write-Ok "Python bundle staged."
}

function Ensure-WinBison {
    $existing = Get-Command "win_bison.exe" -ErrorAction SilentlyContinue
    if ($existing) {
        return $existing.Source
    }

    $existing = Get-Command "win_bison" -ErrorAction SilentlyContinue
    if ($existing) {
        return $existing.Source
    }

    $toolVersion = "2.5.25"
    $toolDir = Join-Path $script:ToolsRoot "winflexbison-$toolVersion"
    $toolExe = Join-Path $toolDir "win_bison.exe"
    if (Test-Path -LiteralPath $toolExe -PathType Leaf) {
        return $toolExe
    }

    $archive = Join-Path $script:DownloadRoot "winflexbison-$toolVersion.zip"
    Download-File -Sources @("https://github.com/lexxmark/winflexbison/releases/download/v$toolVersion/win_flex_bison-$toolVersion.zip") -Destination $archive
    Expand-ArchiveSmart -ArchivePath $archive -Destination $toolDir

    if (-not (Test-Path -LiteralPath $toolExe -PathType Leaf)) {
        throw "win_bison.exe was not found after extracting $archive"
    }

    return $toolExe
}

function Build-MySQLServer {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $bisonExe = Ensure-WinBison
    $buildRel = Join-Path $script:BuildRoot "mysql\release"
    $buildDbg = Join-Path $script:BuildRoot "mysql\debug"
    $stageRel = Join-Path $script:BuildRoot "mysql\stage-release"
    $stageDbg = Join-Path $script:BuildRoot "mysql\stage-debug"
    if ($Clean) {
        foreach ($path in @($buildRel, $buildDbg, $stageRel, $stageDbg)) {
            if (Test-Path -LiteralPath $path -PathType Container) {
                Remove-Item -LiteralPath $path -Recurse -Force
            }
        }
    }
    Ensure-Directory $buildRel
    Ensure-Directory $buildDbg
    Ensure-Directory $stageRel
    Ensure-Directory $stageDbg

    $boostDownloadDir = Join-Path $script:ToolsRoot "mysql-boost"
    Ensure-Directory $boostDownloadDir

    $commonArgs = @(
        "-S", $sourcePath,
        "-G", $Generator,
        "-A", "x64",
        "-DBISON_EXECUTABLE=$bisonExe",
        "-DDOWNLOAD_BOOST=1",
        "-DWITH_BOOST=$boostDownloadDir",
        "-DWITH_ROUTER=OFF",
        "-DWITH_UNIT_TESTS=OFF",
        "-DWITH_MEB=OFF",
        "-DWITH_INNODB_MEMCACHED=OFF",
        "-DWITH_AUTHENTICATION_CLIENT_PLUGINS=ON",
        "-DCMAKE_PREFIX_PATH=$script:BundleDir",
        "-DWITH_SSL=$script:BundleDir"
    )

    $mysqlConfigureReleaseArgs = @("-B", $buildRel) + $commonArgs + @("-DCMAKE_INSTALL_PREFIX=$stageRel")
    Invoke-LoggedCommand -Label "mysql configure release" -FilePath "cmake" -WorkingDirectory $buildRel -Arguments $mysqlConfigureReleaseArgs | Out-Null
    Invoke-LoggedCommand -Label "mysql build release" -FilePath "cmake" -WorkingDirectory $buildRel -Arguments @("--build", $buildRel, "--target", "INSTALL", "--config", "RelWithDebInfo", "--parallel", $Jobs) | Out-Null

    $mysqlConfigureDebugArgs = @("-B", $buildDbg) + $commonArgs + @("-DCMAKE_INSTALL_PREFIX=$stageDbg")
    Invoke-LoggedCommand -Label "mysql configure debug" -FilePath "cmake" -WorkingDirectory $buildDbg -Arguments $mysqlConfigureDebugArgs | Out-Null
    Invoke-LoggedCommand -Label "mysql build debug" -FilePath "cmake" -WorkingDirectory $buildDbg -Arguments @("--build", $buildDbg, "--target", "INSTALL", "--config", "Debug", "--parallel", $Jobs) | Out-Null

    Ensure-Directory (Join-Path $script:BundleDir "include\mysql")
    Copy-DirectoryContent -Source (Join-Path $stageRel "include\mysql") -Destination (Join-Path $script:BundleDir "include\mysql")
    Copy-FirstMatch -SearchRoot $stageRel -Patterns @("mysql.h", "mysql_com.h", "mysql_version.h", "mysqld_error.h", "field_types.h", "mysql_time.h", "mysqlx_*.h", "errmsg.h") -Destination (Join-Path $script:BundleDir "include") -AllowMany | Out-Null

    Copy-FirstMatch -SearchRoot $stageRel -Patterns @("libmysql.dll", "libmysql.lib", "libmysql.pdb") -Destination (Join-Path $script:BundleDir "lib") -AllowMany | Out-Null
    Copy-FirstMatch -SearchRoot $stageDbg -Patterns @("libmysql.dll", "libmysql.lib", "libmysql.pdb") -Destination (Join-Path $script:BundleDir "debug\lib") -AllowMany | Out-Null
    Copy-FirstMatch -SearchRoot $stageRel -Patterns @("mysql.exe", "mysqldump.exe") -Destination (Join-Path $script:BundleDir "bin") -AllowMany | Out-Null
    Copy-FirstMatch -SearchRoot $stageRel -Patterns @("authentication_*.dll", "mysql_native_password.dll") -Destination (Join-Path $script:BundleDir "lib") -AllowMany | Out-Null

    Write-Ok "MySQL client bundle staged."
}

function Build-ConnectorCpp {
    param([pscustomobject]$Dependency)

    Enter-VsBuildEnvironment
    $sourcePath = Get-SourceTree $Dependency
    if ($DownloadOnly) { return }

    $buildRel = Join-Path $script:BuildRoot "connectorcpp\release"
    $buildDbg = Join-Path $script:BuildRoot "connectorcpp\debug"
    $stageRel = Join-Path $script:BuildRoot "connectorcpp\stage-release"
    $stageDbg = Join-Path $script:BuildRoot "connectorcpp\stage-debug"
    if ($Clean) {
        foreach ($path in @($buildRel, $buildDbg, $stageRel, $stageDbg)) {
            if (Test-Path -LiteralPath $path -PathType Container) {
                Remove-Item -LiteralPath $path -Recurse -Force
            }
        }
    }
    Ensure-Directory $buildRel
    Ensure-Directory $buildDbg
    Ensure-Directory $stageRel
    Ensure-Directory $stageDbg

    $commonArgs = @(
        "-S", $sourcePath,
        "-G", $Generator,
        "-A", "x64",
        "-DWITH_JDBC=ON",
        "-DWITH_TESTS=OFF",
        "-DMYSQL_DIR=$script:BundleDir",
        "-DCMAKE_PREFIX_PATH=$script:BundleDir",
        "-DOPENSSL_ROOT_DIR=$script:BundleDir",
        "-DCMAKE_INSTALL_MESSAGE=LAZY"
    )

    $connectorCppConfigureReleaseArgs = @("-B", $buildRel) + $commonArgs + @("-DCMAKE_INSTALL_PREFIX=$stageRel")
    Invoke-LoggedCommand -Label "connectorcpp configure release" -FilePath "cmake" -WorkingDirectory $buildRel -Arguments $connectorCppConfigureReleaseArgs | Out-Null
    Invoke-LoggedCommand -Label "connectorcpp build release" -FilePath "cmake" -WorkingDirectory $buildRel -Arguments @("--build", $buildRel, "--target", "INSTALL", "--config", "RelWithDebInfo", "--parallel", $Jobs) | Out-Null

    $connectorCppConfigureDebugArgs = @("-B", $buildDbg) + $commonArgs + @("-DCMAKE_INSTALL_PREFIX=$stageDbg")
    Invoke-LoggedCommand -Label "connectorcpp configure debug" -FilePath "cmake" -WorkingDirectory $buildDbg -Arguments $connectorCppConfigureDebugArgs | Out-Null
    Invoke-LoggedCommand -Label "connectorcpp build debug" -FilePath "cmake" -WorkingDirectory $buildDbg -Arguments @("--build", $buildDbg, "--target", "INSTALL", "--config", "Debug", "--parallel", $Jobs) | Out-Null

    Ensure-Directory (Join-Path $script:BundleDir "include\cppconn")
    Copy-DirectoryContent -Source (Join-Path $sourcePath "jdbc\cppconn") -Destination (Join-Path $script:BundleDir "include\cppconn")
    Ensure-Directory (Join-Path $script:BundleDir "include\mysql")
    if (Test-Path -LiteralPath (Join-Path $sourcePath "include\mysql\jdbc.h") -PathType Leaf) {
        Copy-Item -LiteralPath (Join-Path $sourcePath "include\mysql\jdbc.h") -Destination (Join-Path $script:BundleDir "include\mysql\jdbc.h") -Force
    }

    Copy-FirstMatch -SearchRoot $stageRel -Patterns @("mysqlcppconn*.lib", "mysqlcppconn*.dll", "mysqlcppconn*.pdb") -Destination (Join-Path $script:BundleDir "lib") -AllowMany | Out-Null
    Copy-FirstMatch -SearchRoot $stageDbg -Patterns @("mysqlcppconn*.lib", "mysqlcppconn*.dll", "mysqlcppconn*.pdb") -Destination (Join-Path $script:BundleDir "debug\lib") -AllowMany | Out-Null

    Write-Ok "Connector/C++ bundle staged."
}

function Get-SelectedDependencies {
    if (-not $script:DepManifest -or $script:DepManifest.Count -eq 0) {
        $script:DepManifest = @(Load-DependencyManifest)
    }

    if (-not $script:SelectedOnly -or $script:SelectedOnly.Count -eq 0) {
        return $script:DepManifest
    }

    $selected = @($script:DepManifest | Where-Object { $script:SelectedOnly -contains $_.Name })
    $missing = @($script:SelectedOnly | Where-Object { $_ -notin $selected.Name })
    if ($missing) {
        throw ("Unknown dependency name(s): {0}" -f ($missing -join ", "))
    }

    return $selected
}

function Check-HostTools {
    Write-Section "Host Tools"
    $required = @("cmake", "tar", "perl", "nasm", "meson", "ninja")
    $missing = @()
    foreach ($tool in $required) {
        if (Test-CommandAvailable $tool) {
            Write-Ok ("{0,-10} {1}" -f $tool, (Get-Command $tool).Source)
        }
        else {
            Write-Fail ("{0,-10} not found" -f $tool)
            $missing += $tool
        }
    }

    if ($missing) {
        throw ("Missing required tools: {0}" -f ($missing -join ", "))
    }

    Enter-VsBuildEnvironment
    foreach ($tool in @("cl.exe", "lib.exe", "cmd.exe", "nmake.exe")) {
        if (Test-CommandAvailable $tool) {
            Write-Ok ("{0,-10} {1}" -f $tool, (Get-Command $tool).Source)
        }
        else {
            throw "Required Visual Studio tool not found after loading the build environment: $tool"
        }
    }
}

function Process-Dependency {
    param([pscustomobject]$Dependency)

    if ($Clean -and (Test-DependencyBuilt $Dependency)) {
        Remove-Item -LiteralPath (Get-StampPath $Dependency) -Force
    }

    if (Test-DependencyBuilt $Dependency) {
        Write-Ok ("Already built - skipping {0}" -f (Get-DependencySlug $Dependency))
        return
    }

    switch ($Dependency.Type) {
        "openssl" { Build-OpenSsl $Dependency }
        "zlib" { Build-ZlibSource $Dependency }
        "libxml2" { Build-LibXml2 $Dependency }
        "cairo" { Build-CairoBundle $Dependency }
        "libzip" { Build-LibZip $Dependency }
        "antlr4" { Build-AntlrRuntime $Dependency }
        "libssh" { Build-LibSsh $Dependency }
        "proj" { Build-Proj $Dependency }
        "gdal" { Build-Gdal $Dependency }
        "header-only" {
            switch ($Dependency.Name) {
                "boost" { Build-BoostHeaders $Dependency }
                "rapidjson" { Build-RapidJsonHeaders $Dependency }
                default { throw "No header-only handler registered for $($Dependency.Name)" }
            }
        }
        "sqlite" { Build-Sqlite $Dependency }
        "vsqlitepp" { Build-Vsqlitepp $Dependency }
        "python" { Build-Python $Dependency }
        "mysql" { Build-MySQLServer $Dependency }
        "connector-cpp" { Build-ConnectorCpp $Dependency }
        default { throw "Unsupported dependency type: $($Dependency.Type)" }
    }

    if (-not $DownloadOnly) {
        Mark-DependencyBuilt $Dependency
    }
}

function Show-Configuration([pscustomobject[]]$Dependencies) {
    Write-Section "Configuration"
    Write-Info ("Project root     : {0}" -f $script:ProjectRoot)
    Write-Info ("Bundle directory : {0}" -f $script:BundleDir)
    Write-Info ("Downloads        : {0}" -f $script:DownloadRoot)
    Write-Info ("Build root       : {0}" -f $script:BuildRoot)
    Write-Info ("Generator        : {0}" -f $Generator)
    Write-Info ("Jobs             : {0}" -f $Jobs)
    if ($Clean) { Write-Warn "--Clean enabled: previous build outputs may be removed." }
    if ($DownloadOnly) { Write-Info "Mode             : download only" }
    if ($BuildOnly) { Write-Info "Mode             : build only" }

    Write-Section ("Dependencies ({0})" -f $Dependencies.Count)
    foreach ($dep in $Dependencies) {
        $source = if ($dep.PSObject.Properties["Urls"] -and $dep.Urls.Count -gt 0) {
            if ($dep.Urls.Count -gt 1) {
                "{0} (+{1} fallback{2})" -f $dep.Urls[0], ($dep.Urls.Count - 1), $(if (($dep.Urls.Count - 1) -eq 1) { "" } else { "s" })
            }
            else {
                $dep.Urls[0]
            }
        }
        elseif ($dep.PSObject.Properties["Url"]) {
            $dep.Url
        }
        elseif ($dep.PSObject.Properties["DisplayUrl"]) {
            $dep.DisplayUrl
        }
        else {
            "-"
        }
        Write-Detail ("{0,-22} {1,-12} {2}" -f (Get-DependencySlug $dep), $dep.Type, $source)
    }
}

function Show-Summary {
    Write-Host ""
    Write-Host "+------------------------------------------------------------------+" -ForegroundColor DarkYellow
    Write-Host "|               3rd-party bundle finished successfully             |" -ForegroundColor Gray
    Write-Host "+------------------------------------------------------------------+" -ForegroundColor DarkYellow
    Write-Host ""
    Write-Detail ("Bundle root        : {0}" -f $script:BundleDir)
    Write-Detail ("Set env var        : MSS_3DPARTY_PATH={0}" -f $script:BundleDir)
    Write-Detail ("Legacy doc alias   : WB_3DPARTY_LIB={0}" -f $script:BundleDir)
    Write-Detail ("Important folders  : bin, include, lib, debug\lib, python")
    Write-Host ""
}

function Main {
    try {
        Write-Banner
        Initialize-BundleLayout

        if ($Clean) {
            Write-Section "Cleaning"
            foreach ($path in @($script:BuildRoot, $script:SourceRoot, $script:StampRoot)) {
                if (Test-Path -LiteralPath $path -PathType Container) {
                    Write-Warn ("Removing {0}" -f $path)
                    Remove-Item -LiteralPath $path -Recurse -Force
                }
                Ensure-Directory $path
            }
        }

        $dependencies = @(Get-SelectedDependencies)
        Show-Configuration -Dependencies $dependencies
        Check-HostTools

        Write-Section "Processing Dependencies"
        $index = 0
        foreach ($dependency in $dependencies) {
            $index++
            Write-StepHeader -Index $index -Total $dependencies.Count -Label (Get-DependencySlug $dependency)
            Process-Dependency -Dependency $dependency
        }

        Show-Summary
    }
    finally {
        Stop-AllActiveProcesses
        Clear-StatusLine
    }
}

Main
