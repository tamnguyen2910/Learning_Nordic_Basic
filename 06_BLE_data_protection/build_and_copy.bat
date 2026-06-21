@echo off
setlocal enabledelayedexpansion

set BOARD=promicro_nrf52840
set NCS_VERSION=3.2.4

set "PATH=C:/ncs/toolchains/fd21892d0f/opt/bin;%PATH%"
set "ZEPHYR_BASE=C:/ncs/v%NCS_VERSION%/zephyr"

for %%i in ("%CD%") do set "PROJECT=%%~ni"

echo === Configuring ===
cmake -GNinja -S . -B build/%PROJECT% -DBOARD=%BOARD%
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Building (UF2 auto-generated) ===
ninja -C build/%PROJECT%
if %errorlevel% neq 0 exit /b %errorlevel%

echo === Copying to E: drive ===
copy /Y build\%PROJECT%\zephyr\zephyr.uf2 E:\%PROJECT%.uf2
if %errorlevel% equ 0 (
    echo Done! Copied to E:\%PROJECT%.uf2
) else (
    echo Failed - is the UF2 bootloader mounted as E:?
)

endlocal
pause