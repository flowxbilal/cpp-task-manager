cmake_minimum_required(VERSION 3.16)
project(CppTaskManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(task_core
    src/Task.cpp
    src/TaskManager.cpp
    src/TaskRepository.cpp
)
target_include_directories(task_core PUBLIC include)

if(MSVC)
    target_compile_options(task_core PRIVATE /W4)
else()
    target_compile_options(task_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(task_manager src/main.cpp)
target_link_libraries(task_manager PRIVATE task_core)

enable_testing()
add_executable(task_manager_tests tests/test_task_manager.cpp)
target_link_libraries(task_manager_tests PRIVATE task_core)
add_test(NAME task_manager_tests COMMAND task_manager_tests)
