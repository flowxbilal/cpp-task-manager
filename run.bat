@echo off
cd /d "%~dp0"
if not exist bin mkdir bin

echo Building Task Manager...
g++ -std=c++17 -Wall -Wextra -Iinclude src\main.cpp src\Task.cpp src\TaskManager.cpp src\TaskRepository.cpp -o bin\task_manager.exe
if errorlevel 1 (
    echo Build failed. Make sure g++ is installed and available in PATH.
    pause
    exit /b 1
)

echo.
bin\task_manager.exe
pause
