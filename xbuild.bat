@ECHO OFF
if "%1"=="clean" del /Q .\output
if "%1"=="clean" echo Cleaned
if "%1"=="clean" exit

if not exist ".\output" mkdir .\output

if not exist ".\output\x86" mkdir .\output\x86
if not exist ".\output\x64" mkdir .\output\x64

if not "%1"=="" goto precise

echo Compiling for user: %username%

if "%username%"=="LP3" goto UserLP3

echo No config for user

exit

:UserLP3
@ECHO OFF
if "%2"=="cursed" goto cursed

set INCLUDE=-I .\include -I .\include\shaders
set LIBS=-lraylib -lopengl32 -lgdi32 -lwinmm

:: Compile for 32 BIT:
PATH = C:\mingw32\bin;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib32 %LIBS% main.c -m32 -o output/x86/main
@ECHO OFF
copy ".\lib32\raylib.dll" .\output\x64
copy "C:\mingw32\bin\libstdc++-6.dll" .\output\x86
copy "C:\mingw32\bin\libgcc_s_sjlj-1.dll" .\output\x86
@REM copy "C:\mingw32\bin\libatomic-1.dll" .\output\x86
@REM copy "C:\mingw32\bin\libwinpthread-1.dll" .\output\x86

:: Compile for 64 BIT:
PATH = C:\mingw64\bin;%PATH%
@ECHO ON
gcc -Wall %INCLUDE% -L .\lib64 %LIBS% main.c -m64 -o output/x64/main
@ECHO OFF
copy ".\lib64\raylib.dll" .\output\x64
copy "C:\mingw64\bin\libstdc++-6.dll" .\output\x64
copy "C:\mingw64\bin\libgcc_s_sjlj-1.dll" .\output\x64
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
copy "C:\mingw64\bin\libgcc_s_sjlj-1.dll" .\
@REM copy "C:\mingw64\bin\libatomic-1.dll" .\output\x64
@REM copy "C:\mingw64\bin\libwinpthread-1.dll" .\output\x64s
@REM copy "C:\mingw64\bin\*.dll" .\output\x64