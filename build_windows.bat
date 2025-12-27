@echo off
echo ========================================
echo    SecurePassGen - Windows Build Script
echo ========================================
echo.

REM Check if MinGW is installed
where gcc >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: MinGW (gcc) not found in PATH!
    echo.
    echo Please install MinGW from:
    echo https://www.mingw-w64.org/
    echo.
    echo Or use MSYS2:
    echo https://www.msys2.org/
    echo.
    pause
    exit /b 1
)

echo Checking MinGW installation...
gcc --version
echo.

REM Create directories
if not exist build mkdir build
if not exist bin mkdir bin

echo Compiling source files...

REM Compile each source file
gcc -c src/main.c -o build/main.o -Wall -Wextra -O2 -D_WIN32
if errorlevel 1 goto error

gcc -c src/password.c -o build/password.o -Wall -Wextra -O2 -D_WIN32
if errorlevel 1 goto error

gcc -c src/security.c -o build/security.o -Wall -Wextra -O2 -D_WIN32
if errorlevel 1 goto error

gcc -c src/ui.c -o build/ui.o -Wall -Wextra -O2 -D_WIN32
if errorlevel 1 goto error

gcc -c src/clipboard.c -o build/clipboard.o -Wall -Wextra -O2 -D_WIN32
if errorlevel 1 goto error

gcc -c src/utils.c -o build/utils.o -Wall -Wextra -O2 -D_WIN32
if errorlevel 1 goto error

gcc -c src/file_ops.c -o build/file_ops.o -Wall -Wextra -O2 -D_WIN32
if errorlevel 1 goto error

echo Linking executable...

REM Link all object files
gcc build/main.o build/password.o build/security.o build/ui.o build/clipboard.o build/utils.o build/file_ops.o -o bin/passgen.exe -luser32 -lkernel32 -lgdi32 -lm
if errorlevel 1 goto error

echo.
echo ========================================
echo    Build Successful!
echo ========================================
echo.
echo Executable: bin\passgen.exe
echo.
echo Usage examples:
echo   bin\passgen.exe --help
echo   bin\passgen.exe -l 16
echo   bin\passgen.exe -l 24 -c 5 -o passwords.txt
echo.
echo To test the program:
echo   bin\passgen.exe --version
echo.
pause
exit /b 0

:error
echo.
echo ========================================
echo    Build Failed!
echo ========================================
echo.
echo Check that all source files exist in src\ directory
echo.
pause
exit /b 1