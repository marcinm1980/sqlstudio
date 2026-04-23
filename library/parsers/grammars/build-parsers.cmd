echo off

if [%1] == [] goto Usage

set "ANTLR_JAR=%MSS_3DPARTY_PATH%\bin\antlr-4.13.2-complete.jar"
if not exist "%ANTLR_JAR%" set "ANTLR_JAR=%MSS_3DPARTY_PATH%\_work\downloads\antlr-4.13.2-complete.jar"

if not exist "%ANTLR_JAR%" (
  echo ANTLR tool jar not found.
  echo Looked for:
  echo   %MSS_3DPARTY_PATH%\bin\antlr-4.13.2-complete.jar
  echo   %MSS_3DPARTY_PATH%\_work\downloads\antlr-4.13.2-complete.jar
  exit /b 1
)

IF %1 == mysql (
  echo Selected MySQL parser
  java -Xmx1024m -jar "%ANTLR_JAR%" -Dlanguage=Cpp -listener -visitor -o ../mysql -package parsers MySQLLexer.g4 MySQLParser.g4
) ELSE (
  echo Unknown parser type %1
)

goto EndOfScript

:Usage
echo "Usage: $0 [mysql]"

:EndOfScript
