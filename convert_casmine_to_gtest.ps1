# Conversion script to migrate casmine tests to Google Test
# This script processes all *_specs.cpp files in the testing/TestingLibrary/library directory

param(
    [string]$SourceDir = "d:\develop\MySQLStudio\testing\TestingLibrary\library",
    [switch]$DryRun = $false
)

function Convert-CasmineToGTest {
    param(
        [string]$FilePath,
        [bool]$DryRun = $false
    )
    
    Write-Host "Processing: $FilePath"
    
    # Read the file content
    $content = Get-Content $FilePath -Raw -Encoding UTF8
    $originalContent = $content
    
    # 1. Replace casmine includes
    $content = $content -replace '#include "casmine\.h"', '#include "gtest/gtest.h"'
    
    # 2. Remove casmine environment setup
    $content = $content -replace '\$ModuleEnvironment\(\)\s*\{\s*\};', ''
    
    # 3. Convert basic expectations first (to avoid conflicts)
    $content = $content -replace '\$expect\(([^)]+)\)\.toEqual\(([^)]+)\)', 'EXPECT_EQ($1, $2)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBe\(([^)]+)\)', 'EXPECT_EQ($1, $2)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBeTrue\(\)', 'EXPECT_TRUE($1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBeFalse\(\)', 'EXPECT_FALSE($1)'
    $content = $content -replace '\$expect\(!([^)]+)\)\.toBeTrue\(\)', 'EXPECT_FALSE($1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBeNull\(\)', 'EXPECT_EQ($1, nullptr)'
    $content = $content -replace '\$expect\(([^)]+)\)\.Not\.toBeNull\(\)', 'EXPECT_NE($1, nullptr)'
    
    # 4. Convert throw expectations  
    $content = $content -replace '\$expect\(([^)]+)\)\.Not\.toThrow\(\)', 'EXPECT_NO_THROW($1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toThrow\(\)', 'EXPECT_THROW($1, std::exception)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toThrowError<([^>]+)>\(([^)]*)\)', 'EXPECT_THROW($1, $2)'
    
    # 5. Convert $fail and $success
    $content = $content -replace '\$fail\(([^)]+)\)', 'FAIL() << $1'
    $content = $content -replace '\$success\(\)', '// Success'
    
    # 6. Convert data-> references to direct member access (manual fix needed)
    $content = $content -replace 'data->', ''
    
    # 7. Convert template function calls with proper syntax
    $content = $content -replace '\.template toThrowError<([^>]+)>', '.toThrowError<$1>'
    
    if ($DryRun) {
        if ($content -ne $originalContent) {
            Write-Host "  Changes detected (dry run mode)"
        } else {
            Write-Host "  No changes needed"
        }
    } else {
        # Write back the content only if changes were made
        if ($content -ne $originalContent) {
            Set-Content -Path $FilePath -Value $content -Encoding UTF8
            Write-Host "  Converted successfully"
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
        Convert-CasmineToGTest -FilePath $file.FullName -DryRun $DryRun
    }
    catch {
        Write-Error "Failed to process $($file.FullName): $_"
    }
}

Write-Host "Processing completed!"
