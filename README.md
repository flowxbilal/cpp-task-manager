# C++ Task Manager

A command-line task manager written in C++17. It demonstrates classes, STL containers, file handling, and a project split across multiple source files.

## Features

- Add tasks with a title, description, category, priority, and optional due date
- View all tasks and see whether they're pending or completed
- Edit tasks without changing their IDs
- Mark tasks as completed or reopen them
- Delete tasks with confirmation
- Search by title, description, or category (case-insensitive)
- Filter by pending or completed status
- Save changes automatically and load tasks when the program starts
- Validate input, including calendar dates and numeric menu choices

## Project layout

```text
cpp-task-manager/
├── include/
│   ├── Task.h
│   ├── TaskManager.h
│   └── TaskRepository.h
├── src/
│   ├── main.cpp
│   ├── Task.cpp
│   ├── TaskManager.cpp
│   └── TaskRepository.cpp
├── tests/
│   └── test_task_manager.cpp
├── CMakeLists.txt
├── run.bat
├── .gitignore
└── README.md
```

## Build and run

### Quick start on Windows

If you already have **g++** installed and working in VS Code, just double-click `run.bat` in the project folder. It compiles the code and runs the app. (It will not work without a C++ compiler.)

### Build with CMake

You'll need **CMake 3.16+** and a compiler with **C++17** support (GCC, Clang, or MSVC).

In the VS Code terminal, open the **project's main folder**, then run:

```bash
cmake -S . -B build
cmake --build build
```

**On Windows PowerShell (MinGW/GCC)**, you can select the MinGW Makefiles generator if available:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\task_manager.exe
```

**On Windows with Visual Studio's C++ build tools** (default multi-config generator):

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\task_manager.exe
```

**On Linux/macOS:**

```bash
./build/task_manager
```

If CMake isn't installed yet, you can compile directly from the project folder with g++:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Task.cpp src/TaskManager.cpp src/TaskRepository.cpp -o task_manager
```

Then run `./task_manager` (Linux/macOS) or `.\task_manager.exe` (Windows PowerShell).

> Run the program from the project root folder. The app creates `data/tasks.db` relative to the current working directory, which is intentionally excluded from Git.

## Example

```text
==================================
         TASK MANAGER
==================================
1. Add task
2. View all tasks
3. Edit task
4. Complete / reopen task
5. Delete task
6. Search tasks
7. Filter by status
8. Exit
Choose an option: 1

New task
Title: Finish DSA assignment
Description: Practice linked lists
Category (default: General): Study
Priority: 1 = Low, 2 = Medium, 3 = High
Priority (default: 2): 3
Due date (YYYY-MM-DD, blank for none): 2027-02-15
Task added successfully.
```

During editing, pressing Enter keeps a field's existing value. Enter `-` in the due-date field to clear it.

## Running tests

```bash
ctest --test-dir build --output-on-failure
```

Tests cover adding tasks, editing, searching, status changes, deletion, validation, and saving/loading tasks.

## How the code is organized

- `Task` holds data for one task.
- `TaskManager` contains the actions that change or retrieve tasks.
- `TaskRepository` reads and writes the local task file.
- `main.cpp` handles the menu and user input.

Task IDs remain stable when tasks are deleted, so deleting Task #1 doesn't change the ID of Task #2.

## Limitations / possible improvements

- This is a **local, single-user** application, not a shared online task service.
- Tasks are stored in a custom text format instead of a database.
- The console menu is intended for learning, not as a production UI.
- Possible future improvements: task sorting, overdue reminders, CSV export, and a database backend.
