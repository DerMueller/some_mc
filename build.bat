@ECHO OFF
if "%1"=="clean" del /Q .\output
if "%1"=="clean" echo Cleaned
if "%1"=="clean" exit

if not exist ".\output" mkdir .\output

if not exist ".\output\x86" mkdir .\output\x86
if not exist ".\output\x64" mkdir .\output\x64

echo Compiling for user: %username%

@ECHO OFF

set MINGW64_BIN=D:\Programme\mingw64\bin
set MINGW32_BIN=D:\Programme\mingw32\bin

set INCLUDE=-I .\include -I .\include\shaders
set LIBS=-lraylib -lopengl32 -lgdi32 -lwinmm 

if "%1"=="double_pen" goto double_pendulum
if "%1"=="triple_pen" goto triple_pendulum
if "%1"=="rec_tree" goto shapes_recursive_tree
if "%1"=="custom" goto custom

:: Compile for 32 BIT:
PATH = %MINGW32_BIN%;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib32 src/main.c %LIBS% -m32 -o output/x86/main
@ECHO OFF
copy ".\lib64\raylib.dll" .\output\x86
copy "%MINGW32_BIN%\libstdc++-6.dll" .\output\x86
@REM  copy "%MINGW32_BIN%\libgcc_s_sjlj-1.dll" .\output\x86
@REM libgcc_s_sjls is only available in a sjlj compiled build of mingw - we do not do that anymore.
@REM copy "C:\mingw32\bin\libatomic-1.dll" .\output\x86
@REM copy "C:\mingw32\bin\libwinpthread-1.dll" .\output\x86

:: Compile for 64 BIT:
PATH = %MINGW64_BIN%;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib64 src/main.c %LIBS% -m64 -o output/x64/main
@ECHO OFF
copy ".\lib64\raylib.dll" .\output\x64
copy "%MINGW64_BIN%\libstdc++-6.dll" .\output\x64
@REM  copy "%MINGW64_BIN%\libgcc_s_sjlj-1.dll" .\output\x64
@REM libgcc_s_sjls is only available in a sjlj compiled build of mingw - we do not do that anymore.
@REM copy "C:\mingw64\bin\libatomic-1.dll" .\output\x64
@REM copy "C:\mingw64\bin\libwinpthread-1.dll" .\output\x64s
@REM copy "C:\mingw64\bin\*.dll" .\output\x64

exit


:precise

if not exist ".\output\x64p" mkdir .\output\x64p

set INCLUDE=-I .\include -I .\include\shaders -I .\
set LIBS=-lraylib -lopengl32 -lgdi32 -lwinmm

PATH = C:\mingw64\bin;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib64 %LIBS% %1 -m64 -o main
@ECHO OFF
copy ".\lib64\raylib.dll" .\
copy "C:\mingw64\bin\libstdc++-6.dll" .\
@REM  copy "C:\mingw64\bin\libgcc_s_sjlj-1.dll" .\
@REM libgcc_s_sjls is only available in a sjlj compiled build of mingw - we do not do that anymore.
@REM copy "C:\mingw64\bin\libatomic-1.dll" .\output\x64
@REM copy "C:\mingw64\bin\libwinpthread-1.dll" .\output\x64s
@REM copy "C:\mingw64\bin\*.dll" .\output\x64


:double_pendulum

PATH = %MINGW64_BIN%;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib64 src/shapes_double_pendulum.c %LIBS% -m64 -o output/x64/main
@ECHO OFF

exit

:triple_pendulum

PATH = %MINGW64_BIN%;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib64 src/shapes_triple_pendulum.c %LIBS% -m64 -o output/x64/main
@ECHO OFF

exit


:shapes_recursive_tree

PATH = %MINGW64_BIN%;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib64 src/shapes_recursive_tree.c %LIBS% -m64 -o output/x64/main
@ECHO OFF

exit


:custom

PATH = %MINGW64_BIN%;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib64 src/%~2 %LIBS% -m64 -o output/x64/main
@ECHO OFF

exit