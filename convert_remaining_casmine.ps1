# PowerShell script to convert remaining casmine syntax to Google Test
param(
    [string]$FilePath = ""
)

function Convert-CasmineToGoogleTest {
    param(
        [string]$Path
    )
    
    Write-Host "Converting $Path..."
    $content = Get-Content $Path -Raw -Encoding UTF8
    
    # Convert $expect patterns to EXPECT_* macros
    $content = $content -replace '\$expect\(([^)]+)\)\.toEqual\(([^,)]+)(?:,\s*"[^"]*")?\)', 'EXPECT_EQ($2, $1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBe\(([^,)]+)(?:,\s*"[^"]*")?\)', 'EXPECT_EQ($2, $1)'  
    $content = $content -replace '\$expect\(([^)]+)\)\.toBeTrue\((?:"[^"]*")?\)', 'EXPECT_TRUE($1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBeFalse\((?:"[^"]*")?\)', 'EXPECT_FALSE($1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBeNull\((?:"[^"]*")?\)', 'EXPECT_EQ(nullptr, $1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.not\.toBeNull\((?:"[^"]*")?\)', 'EXPECT_NE(nullptr, $1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.Not\.toBe\(([^,)]+)(?:,\s*"[^"]*")?\)', 'EXPECT_NE($2, $1)'
    $content = $content -replace '\$expect\(([^)]+)\)\.Not\.toEqual\(([^,)]+)(?:,\s*"[^"]*")?\)', 'EXPECT_NE($2, $1)'
    
    # Convert with comments
    $content = $content -replace '\$expect\(([^)]+)\)\.toEqual\(([^,)]+),\s*"([^"]*)"\)', 'EXPECT_EQ($2, $1) /* "$3" */'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBe\(([^,)]+),\s*"([^"]*)"\)', 'EXPECT_EQ($2, $1) /* "$3" */'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBeTrue\("([^"]*)"\)', 'EXPECT_TRUE($1) /* "$2" */'
    $content = $content -replace '\$expect\(([^)]+)\)\.toBeFalse\("([^"]*)"\)', 'EXPECT_FALSE($1) /* "$2" */'
    $content = $content -replace '\$expect\(([^)]+)\)\.Not\.toBe\(([^,)]+),\s*"([^"]*)"\)', 'EXPECT_NE($2, $1) /* "$3" */'
    $content = $content -replace '\$expect\(([^)]+)\)\.Not\.toEqual\(([^,)]+),\s*"([^"]*)"\)', 'EXPECT_NE($2, $1) /* "$3" */'
    
    Set-Content $Path $content -Encoding UTF8
    Write-Host "Converted $Path successfully"
}

if ($FilePath -ne "") {
    # Convert single file
    if (Test-Path $FilePath) {
        Convert-CasmineToGoogleTest -Path $FilePath
    } else {
        Write-Error "File not found: $FilePath"
    }
} else {
    # Convert all files with casmine references
    $files = @(
        "d:\develop\MySQLStudio\testing\test-suite\tests\backend\wbpublic\grtdb\editor_table_specs.cpp"
    )
    
    foreach ($file in $files) {
        if (Test-Path $file) {
            Convert-CasmineToGoogleTest -Path $file
        } else {
            Write-Host "File not found: $file"
        }
    }
}

Write-Host "Conversion complete!"