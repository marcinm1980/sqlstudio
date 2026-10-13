#requires -Version 7.0
# Copyright (c) 2026 dev4fun. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is designed to work with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms, as
# designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have either included with
# the program or referenced in the documentation.
# This program is distributed in the hope that it will be useful,  but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
# the GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

<#
.SYNOPSIS
Build the React frontend and/or the native host on Windows, macOS, or Linux.
.EXAMPLE
pwsh ./build.ps1
.EXAMPLE
pwsh ./build.ps1 -Target React
.EXAMPLE
pwsh ./build.ps1 -Target Native -Configuration Debug
.EXAMPLE
pwsh ./build.ps1 -Platform Windows -InstallDependencies
.EXAMPLE
pwsh ./build.ps1 -Start
#>
[CmdletBinding()]
param(
    [ValidateSet('All', 'React', 'Native')]
    [string] $Target = 'All',

    [ValidateSet('Auto', 'Windows', 'macOS', 'Linux')]
    [string] $Platform = 'Auto',

    [ValidateSet('Debug', 'Release')]
    [string] $Configuration = 'Release',

    [switch] $InstallDependencies,

    [switch] $Start
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Require-Tool([string] $Name, [string] $Instructions) {
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        throw "Required tool '$Name' was not found. $Instructions"
    }
}

function Invoke-BuildTool([string] $Name, [string[]] $Arguments) {
    Write-Host "> $Name $($Arguments -join ' ')" -ForegroundColor Cyan
    & $Name @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$Name failed with exit code $LASTEXITCODE."
    }
}

$hostPlatform = if ($IsWindows) { 'Windows' } elseif ($IsMacOS) { 'macOS' } elseif ($IsLinux) { 'Linux' } else { throw 'Unsupported operating system.' }
$selectedPlatform = if ($Platform -eq 'Auto') { $hostPlatform } else { $Platform }
$buildReact = $Target -in 'All', 'React'
$buildNative = $Target -in 'All', 'Native'

# Resolve every project path relative to this script, regardless of caller cwd.
Push-Location -LiteralPath $PSScriptRoot
try {
    if ($buildNative) {
        if ($selectedPlatform -ne $hostPlatform) {
            throw "Build the $selectedPlatform host on $selectedPlatform. This machine runs $hostPlatform; native cross-compilation is not configured. Use -Target React to build only the shared frontend."
        }
        switch ($selectedPlatform) {
            'Windows' { Require-Tool 'dotnet' 'Install the .NET 10 SDK.' }
            'macOS' { Require-Tool 'swift' 'Install Xcode Command Line Tools with Swift 5.9 or newer.' }
            'Linux' {
                Require-Tool 'meson' 'Install Meson, Ninja, a C compiler, GTK 4 and WebKitGTK 6 development packages.'
                Require-Tool 'ninja' 'Install Ninja.'
            }
        }
    }

    if ($buildReact -or $Start) {
        Require-Tool 'node' 'Install a Node.js version supported by web/package.json and Vite.'
        $npm = if ($IsWindows) { 'npm.cmd' } else { 'npm' }
        Require-Tool $npm 'Install Node.js with npm.'
        if ($InstallDependencies -or -not (Test-Path -LiteralPath 'web/node_modules')) {
            Invoke-BuildTool $npm @('--prefix', 'web', 'ci')
        }
        if ($buildReact) {
            Invoke-BuildTool $npm @('--prefix', 'web', 'run', 'build')
            Write-Host "React output: $(Join-Path $PSScriptRoot 'web/dist')" -ForegroundColor Green
        }
    }

    if ($buildNative) {
        if (-not $Start -and -not (Test-Path -LiteralPath 'web/dist/index.html')) {
            throw 'React output is missing. Run ./build.ps1 -Target React first, or use -Target All.'
        }
        $nativeConfig = $Configuration.ToLowerInvariant()
        switch ($selectedPlatform) {
            'Windows' {
                Invoke-BuildTool 'dotnet' @('build', 'hosts/windows/WindowsHost.csproj', '--configuration', $Configuration, '--nologo')
                $output = "hosts/windows/bin/$Configuration/net10.0-windows/WindowsHost.exe"
            }
            'macOS' {
                Invoke-BuildTool 'swift' @('build', '--package-path', 'hosts/macos', '--configuration', $nativeConfig)
                $output = "hosts/macos/.build/$nativeConfig/endless-control-mac"
            }
            'Linux' {
                $buildDirectory = "build/linux/$nativeConfig"
                $setup = @('setup', $buildDirectory, 'hosts/linux', '--buildtype', $nativeConfig)
                if (Test-Path -LiteralPath "$buildDirectory/meson-private/coredata.dat") {
                    $setup += '--reconfigure'
                }
                Invoke-BuildTool 'meson' $setup
                Invoke-BuildTool 'meson' @('compile', '-C', $buildDirectory)
                $output = "$buildDirectory/endless-control"
            }
        }
        Write-Host "Native output: $(Join-Path $PSScriptRoot $output)" -ForegroundColor Green
        if ($selectedPlatform -ne 'Windows') {
            Write-Host "When launching, set ENDLESS_CONTROL_WEB_ROOT to $(Join-Path $PSScriptRoot 'web/dist')."
        }
    }

    if ($Start) {
        $launchArguments = @{}
        if ($buildNative) { $launchArguments.NativeExecutable = Join-Path $PSScriptRoot $output }
        & (Join-Path $PSScriptRoot 'scripts/start-dev.ps1') @launchArguments
    }
}
catch {
    Write-Error $_ -ErrorAction Continue
    exit 1
}
finally {
    Pop-Location
}
