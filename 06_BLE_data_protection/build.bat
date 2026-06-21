@echo off
setlocal enabledelayedexpansion

rem ---- Cấu hình (sửa nếu cần) ----
set BOARD=promicro_nrf52840
set NCS_VERSION=3.2.4
rem -----------------------------------

set "PATH=C:/ncs/toolchains/fd21892d0f/opt/bin;%PATH%"
set "ZEPHYR_BASE=C:/ncs/v%NCS_VERSION%/zephyr"

rem Lấy tên thư mục hiện tại làm tên project
for %%i in ("%CD%") do set "PROJECT=%%~ni"

cmake -GNinja -S . -B build/%PROJECT% -DBOARD=%BOARD%
if %errorlevel% neq 0 exit /b %errorlevel%
ninja -C build/%PROJECT%

endlocal
pause
