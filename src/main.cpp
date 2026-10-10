#include "TaskManager.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    bool readLine(const std::string& prompt, std::string& value)
    {
        std::cout << prompt;
        return static_cast<bool>(std::getline(std::cin, value));
    }

    bool parseNumber(const std::string& input, int& result)
    {
        std::istringstream stream(input);
        char extra;
        return static_cast<bool>(stream >> result) && !(stream >> extra);
    }

    int askNumber(const std::string& prompt, int minimum, int maximum)
    {
        std::string text;
        while(readLine(prompt, text))
        {
            int number;
            if(parseNumber(text, number) && number >= minimum && number <= maximum)
                return number;
            std::cout << "Please enter a number between " << minimum
                      << " and " << maximum << ".\n";
        }
        return -1;
    }

    void showTask(const Task& task)
    {
        std::cout << "\n#" << task.id << "  " << task.title << "\n"
                  << "   Status: " << (task.completed ? "Completed" : "Pending")
                  << " | Priority: " << priorityName(task.priority)
                  << " | Category: " << task.category << "\n";
        if(!task.dueDate.empty())
            std::cout << "   Due: " << task.dueDate << "\n";
        if(!task.description.empty())
            std::cout << "   Notes: " << task.description << "\n";
    }

    void showTasks(const std::vector<Task>& tasks)
    {
        if(tasks.empty())
        {
            std::cout << "\nNo tasks found.\n";
            return;
        }

        int completed = 0;
        for(const Task& task : tasks)
        {
            showTask(task);
            if(task.completed)
                ++completed;
        }
        std::cout << "\nShowing " << tasks.size() << " task(s), "
                  << completed << " completed.\n";
    }

    bool readTaskDetails(Task& task, bool editing)
    {
        std::string text;
        std::cout << "\n" << (editing ? "Edit task (Enter keeps the current value)" : "New task") << "\n";

        if(!readLine("Title" + std::string(editing ? " [" + task.title + "]" : "") + ": ", text))
            return false;
        if(!editing || !text.empty())
            task.title = text;

        if(!readLine("Description" + std::string(editing ? " [" + task.description + "]" : "") + ": ", text))
            return false;
        if(!editing || !text.empty())
            task.description = text;

        if(!readLine("Category" + std::string(editing ? " [" + task.category + "]" : " (default: General)") + ": ", text))
            return false;
        if(!text.empty())
            task.category = text;

        std::cout << "Priority: 1 = Low, 2 = Medium, 3 = High\n";
        while(true)
        {
            if(!readLine("Priority" + std::string(editing ? " [" + priorityName(task.priority) + "]" : " (default: 2)") + ": ", text))
                return false;
            if(text.empty())
                break;
            int value;
            if(parseNumber(text, value) && value >= 1 && value <= 3)
            {
                task.priority = static_cast<Priority>(value);
                break;
            }
            std::cout << "Choose 1, 2 or 3.\n";
        }

        while(true)
        {
            if(!readLine("Due date (YYYY-MM-DD, blank for none)" +
                         std::string(editing ? " [" + (task.dueDate.empty() ? "none" : task.dueDate) + "]" : "") + ": ", text))
                return false;
            text = trim(text);
            if(editing && text.empty())
                break;
            if(text == "-")
                text.clear();
            if(validDate(text))
            {
                task.dueDate = text;
                break;
            }
            std::cout << "Please enter a valid date, for example 2026-12-15.\n";
        }
        return true;
    }

    void showMenu()
    {
        std::cout << "\n==================================\n"
                  << "         TASK MANAGER\n"
                  << "==================================\n"
                  << "1. Add task\n"
                  << "2. View all tasks\n"
                  << "3. Edit task\n"
                  << "4. Complete / reopen task\n"
                  << "5. Delete task\n"
                  << "6. Search tasks\n"
                  << "7. Filter by status\n"
                  << "8. Exit\n";
    }
}

int main()
{
    TaskManager manager(TaskRepository("data/tasks.db"));
    std::string error;
    if(!manager.initialize(error))
    {
        std::cerr << "Cannot start Task Manager: " << error << "\n";
        return 1;
    }

    while(true)
    {
        showMenu();
        const int choice = askNumber("Choose an option: ", 1, 8);
        if(choice == -1 || choice == 8)
        {
            std::cout << "Goodbye!\n";
            break;
        }

        if(choice == 1)
        {
            Task task;
            if(!readTaskDetails(task, false))
                break;
            if(manager.add(task, error))
                std::cout << "Task added successfully.\n";
            else
                std::cout << "Error: " << error << "\n";
        }
        else if(choice == 2)
        {
            showTasks(manager.all());
        }
        else if(choice == 3)
        {
            const int id = askNumber("Task ID to edit: ", 1, 1000000000);
            if(id == -1)
                break;
            const Task* existing = manager.find(id);
            if(!existing)
            {
                std::cout << "Task not found.\n";
                continue;
            }
            Task updated = *existing;
            if(!readTaskDetails(updated, true))
                break;
            if(manager.edit(id, updated, error))
                std::cout << "Task updated.\n";
            else
                std::cout << "Error: " << error << "\n";
        }
        else if(choice == 4)
        {
            const int id = askNumber("Task ID: ", 1, 1000000000);
            if(id == -1)
                break;
            const Task* task = manager.find(id);
            if(!task)
            {
                std::cout << "Task not found.\n";
                continue;
            }
            const bool nextStatus = !task->completed;
            if(manager.setCompleted(id, nextStatus, error))
                std::cout << (nextStatus ? "Task completed.\n" : "Task reopened.\n");
            else
                std::cout << "Error: " << error << "\n";
        }
        else if(choice == 5)
        {
            const int id = askNumber("Task ID to delete: ", 1, 1000000000);
            if(id == -1)
                break;
            const Task* task = manager.find(id);
            if(!task)
            {
                std::cout << "Task not found.\n";
                continue;
            }
            showTask(*task);
            std::string confirmation;
            if(!readLine("Delete this task? (y/N): ", confirmation))
                break;
            if(confirmation == "y" || confirmation == "Y")
            {
                if(manager.remove(id, error))
                    std::cout << "Task deleted.\n";
                else
                    std::cout << "Error: " << error << "\n";
            }
            else
                std::cout << "Deletion cancelled.\n";
        }
        else if(choice == 6)
        {
            std::string query;
            if(!readLine("Search by title, notes, or category: ", query))
                break;
            showTasks(manager.search(query));
        }
        else if(choice == 7)
        {
            const int status = askNumber("1. Pending  2. Completed: ", 1, 2);
            if(status == -1)
                break;
            showTasks(manager.byStatus(status == 2));
        }
    }
    return 0;
}
