@echo off
setlocal

cd /d "%~dp0"
node -e "process.argv.splice(1, 0, 'tools/build.js'); eval(require('fs').readFileSync('tools/build.js', 'utf8'))" %*
exit /b %ERRORLEVEL%
