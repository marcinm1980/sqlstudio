# Advanced Casmine to Google Test conversion script

param(
    [string]$SourceDir = "d:\develop\MySQLStudio\testing\TestingLibrary\library",
    [switch]$DryRun = $false
)

function Convert-ExpectStatements {
    param([string]$Content)
    
    # Convert $expect statements with proper escaping and multiple pattern matching
    $Content = $Content -replace '\$expect\(([^)]+)\)\.toEqual\(([^)]+)\)', 'EXPECT_EQ($1, $2)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\.toBe\(([^)]+)\)', 'EXPECT_EQ($1, $2)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\.toBeTrue\(\)', 'EXPECT_TRUE($1)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\.toBeFalse\(\)', 'EXPECT_FALSE($1)'
    $Content = $Content -replace '\$expect\(!([^)]+)\)\.toBeTrue\(\)', 'EXPECT_FALSE($1)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\.toBeNull\(\)', 'EXPECT_EQ($1, nullptr)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\.Not\.toBeNull\(\)', 'EXPECT_NE($1, nullptr)'
    
    # Handle more complex multi-line expects
    $Content = $Content -replace '\$expect\(([^)]+)\)\s*\.\s*toBe\(([^)]+)\)', 'EXPECT_EQ($1, $2)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\s*\.\s*toEqual\(([^)]+)\)', 'EXPECT_EQ($1, $2)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\s*\.\s*toBeTrue\(\)', 'EXPECT_TRUE($1)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\s*\.\s*toBeFalse\(\)', 'EXPECT_FALSE($1)'
    
    # Convert throw expectations  
    $Content = $Content -replace '\$expect\(([^)]+)\)\.Not\.toThrow\(\)', 'EXPECT_NO_THROW($1)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\.toThrow\(\)', 'EXPECT_THROW($1, std::exception)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\.toThrowError<([^>]+)>\("([^"]+)"\)', 'EXPECT_THROW($1, $2)'
    $Content = $Content -replace '\$expect\(([^)]+)\)\.toThrowError<([^>]+)>\(([^)]*)\)', 'EXPECT_THROW($1, $2)'
    
    # Convert other casmine directives
    $Content = $Content -replace '\$fail\(([^)]+)\)', 'FAIL() << $1'
    $Content = $Content -replace '\$success\(\)', '// Success'
    
    return $Content
}

function Convert-CasmineAdvanced {
    param(
        [string]$FilePath,
        [bool]$DryRun = $false
    )
    
    Write-Host "Processing: $FilePath"
    
    $content = Get-Content $FilePath -Raw -Encoding UTF8
    $originalContent = $content
    
    # 1. Basic conversions
    $content = Convert-ExpectStatements $content
    
    # 2. Remove data-> references
    $content = $content -replace 'data->', ''
    
    if ($DryRun) {
        if ($content -ne $originalContent) {
            # Count remaining $expect statements
            $expectCount = ([regex]::Matches($content, '\$expect')).Count
            Write-Host "  Changes detected, $expectCount remaining \$expect statements (dry run mode)"
        } else {
            Write-Host "  No changes needed"
        }
    } else {
        if ($content -ne $originalContent) {
            Set-Content -Path $FilePath -Value $content -Encoding UTF8
            $expectCount = ([regex]::Matches($content, '\$expect')).Count
            Write-Host "  Converted successfully, $expectCount remaining \$expect statements"
        } else {
            Write-Host "  No changes needed"
        }
    }
}

# Find all test files
$testFiles = Get-ChildItem -Path $SourceDir -Recurse -Filter "*_specs.cpp"

Write-Host "Found $($testFiles.Count) test files to convert"
if ($DryRun) {
    Write-Host "(Running in DRY RUN mode - no files will be modified)"
}

foreach ($file in $testFiles) {
    try {
        Convert-CasmineAdvanced -FilePath $file.FullName -DryRun $DryRun
    }
    catch {
        Write-Error "Failed to process $($file.FullName): $_"
    }
}

Write-Host "Processing completed!"
