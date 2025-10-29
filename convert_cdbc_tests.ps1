$files = @(
    "d:\develop\MySQLStudio\testing\TestingLibrary\library\cdbc\dbc_general_specs.cpp",
    "d:\develop\MySQLStudio\testing\TestingLibrary\library\cdbc\dbc_connection_specs.cpp", 
    "d:\develop\MySQLStudio\testing\TestingLibrary\library\cdbc\dbc_metadata_specs.cpp",
    "d:\develop\MySQLStudio\testing\TestingLibrary\library\cdbc\dbc_result_set_specs.cpp",
    "d:\develop\MySQLStudio\testing\TestingLibrary\library\cdbc\dbc_statement_specs.cpp"
)

foreach ($filePath in $files) {
    if (Test-Path $filePath) {
        Write-Host "Converting $filePath..."
        $content = Get-Content $filePath -Raw -Encoding UTF8
        
        # Replace basic casmine syntax patterns
        $content = $content -replace '\$describe\("([^"]+)"\)\s*{', 'class $1Test : public ::testing::Test {
protected:
    void SetUp() override {'
        
        $content = $content -replace '\$beforeAll\(\[\&\]\(\)\s*{', ''
        $content = $content -replace '\$TestData\s*{([^}]+)}', 'private:$1'
        
        # Replace $it patterns with TEST_F - need to extract class name
        $content = $content -replace '\$it\("([^"]+)",\s*\[\&\]\(\)\s*{', 'TEST_F(UnknownTest, $1) {'
        
        # Replace expectation patterns
        $content = $content -replace '\$expect\(([^)]+)\)\.toBe\(([^,)]+)(?:,\s*"[^"]*")?\)', 'EXPECT_EQ($2, $1)'
        $content = $content -replace '\$expect\(([^)]+)\)\.toBeTrue\(\)', 'EXPECT_TRUE($1)'
        $content = $content -replace '\$expect\(([^)]+)\)\.toBeFalse\(\)', 'EXPECT_FALSE($1)'
        $content = $content -replace '\$expect\(([^)]+)\)\.toBeNull\(\)', 'EXPECT_EQ(nullptr, $1)'
        $content = $content -replace '\$expect\(([^)]+)\)\.not\.toBeNull\(\)', 'EXPECT_NE(nullptr, $1)'
        $content = $content -replace '\$expect\(([^)]+)\)\.toThrow\(\)', 'EXPECT_THROW($1, std::exception)'
        
        Set-Content $filePath $content -Encoding UTF8
        Write-Host "Converted $filePath"
    } else {
        Write-Host "File not found: $filePath"
    }
}
