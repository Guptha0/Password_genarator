@echo off
echo Building SecurePassGen...
echo.

echo Checking for gcc...
where gcc
if %errorlevel% neq 0 (
    echo GCC not found. Please install MinGW.
    pause
    exit /b 1
)

echo Creating directories...
if not exist build mkdir build
if not exist bin mkdir binls 

echo Compiling...
gcc -c src/main.c -o build/main.o -Wall -Wextra -O2
gcc -c src/password.c -o build/password.o -Wall -Wextra -O2
gcc -c src/security.c -o build/security.o -Wall -Wextra -O2
gcc -c src/ui.c -o build/ui.o -Wall -Wextra -O2
gcc -c src/clipboard.c -o build/clipboard.o -Wall -Wextra -O2
gcc -c src/utils.c -o build/utils.o -Wall -Wextra -O2
gcc -c src/file_ops.c -o build/file_ops.o -Wall -Wextra -O2

echo Linking...
gcc build/main.o build/password.o build/security.o build/ui.o build/clipboard.o build/utils.o build/file_ops.o -o bin/passgen.exe -lm

echo.
echo Done! Executable created: bin\passgen.exe
echo.
echo To run: bin\passgen.exe
pause