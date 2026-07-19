@echo off
REM Build the custom 4coder layer (custom_4coder.dll) on Windows.
REM
REM Usage:
REM   build.bat <path-to-4coder-custom-dir> [output-dir]
REM
REM <path-to-4coder-custom-dir> is the `custom` directory of a 4coder source
REM tree / distribution (ships 4coder_default_include.cpp,
REM 4coder_metadata_generator.cpp and generated\). See README.md.
REM
REM Run this from a Visual Studio x64 developer command prompt (so cl.exe is on
REM PATH), or ensure the MSVC environment is set up beforehand.

setlocal
set repo=%~dp0
set source=%repo%custom\4coder_config.cpp

set code_home=%~1
if "%code_home%" == "" set code_home=%FCODER_CUSTOM%
if "%code_home%" == "" (
    echo usage: build.bat ^<path-to-4coder\code\custom^> [output-dir]
    echo    or: set FCODER_CUSTOM=^<path^> ^&^& build.bat
    exit /b 1
)

set out=%~2
if "%out%" == "" set out=%repo%build
if not exist "%out%" mkdir "%out%"
pushd "%out%"

set opts=/W4 /wd4310 /wd4100 /wd4201 /wd4505 /wd4996 /wd4127 /wd4510 /wd4512 /wd4610 /wd4457 /WX
set opts=%opts% /GR- /nologo /FC
set opts=%opts% -I"%code_home%"
set opts=%opts% /D OS_WINDOWS=1 /D OS_LINUX=0 /D OS_MAC=0
set opts=%opts% /Zi

set preproc_file=4coder_command_metadata.i
set meta_opts=/P /Fi"%preproc_file%" /DMETA_PASS

set build_dll=/LD /link /INCREMENTAL:NO /OPT:REF /RELEASE
set build_dll=%build_dll% /EXPORT:get_version /EXPORT:init_apis

call cl %opts% %meta_opts% "%source%"
call cl %opts% "%code_home%\4coder_metadata_generator.cpp" /Femetadata_generator
metadata_generator -R "%code_home%" "%out%\%preproc_file%"
call cl %opts% "%source%" /Fecustom_4coder %build_dll%

del metadata_generator* 2>nul
del *.exp 2>nul
del *.obj 2>nul
del *.lib 2>nul
del %preproc_file% 2>nul
popd
echo Built %out%\custom_4coder.dll
endlocal
