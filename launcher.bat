@echo off
setlocal enabledelayedexpansion

:: Set Gurobi environment path
::set GUROBI_HOME=C:\gurobi11.0.1
::set PATH=%GUROBI_HOME%\bin;%PATH%

:: Create Results directory if it doesn't exist
if not exist "Results" mkdir Results

:: Loop through all config files
for %%F in (config_files\*.txt) do (
    echo --------------------------------------
    echo Processing config file: %%~nxF

    :: Read the command line
    set "cmdline="
    set /p cmdline=<%%F
    echo Command: !cmdline!

    :: Extract instance file path (2nd token after "Instances")
    for /f "tokens=2" %%I in ("!cmdline!") do (
        set "instance_path=%%I"
    )
    for %%A in (!instance_path!) do set "instance_file=%%~nxA"
    for %%B in (!instance_file!) do set "instance_base=%%~nB"

    :: Extract the -m value
    set "m_value="
    set "last_token="
    for %%K in (!cmdline!) do (
        if "!last_token!"=="-m" (
            set "m_value=%%K"
        )
        set "last_token=%%K"
    )

    echo Instance: !instance_base!, m: !m_value!

    :: Rename and move output files with prefix "out" and suffix
    set "suffix=_!instance_base!_m!m_value!"
	
    :: Run the program
    asignacion.exe !cmdline! >out!suffix!.txt 2>out2!suffix!.txt

    :: Clear temp vars
    set "last_token="
)

echo --------------------------------------
echo All tasks completed.
pause
