#include "TaskRepository.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_set>

TaskRepository::TaskRepository(std::filesystem::path filePath)
    : filePath_(std::move(filePath))
{
}

bool TaskRepository::load(std::vector<Task>& tasks, std::string& error) const
{
    tasks.clear();
    error.clear();

    std::error_code ec;
    if(!std::filesystem::exists(filePath_, ec))
    {
        if(ec)
        {
            error = "Cannot access task data: " + ec.message();
            return false;
        }
        return true;
    }

    std::ifstream input(filePath_);
    if(!input)
    {
        error = "Cannot open " + filePath_.string();
        return false;
    }

    std::string line;
    if(!std::getline(input, line) || line != "TASKS_V1")
    {
        error = "The task data file has an invalid format.";
        return false;
    }

    std::unordered_set<int> ids;
    int lineNumber = 1;
    while(std::getline(input, line))
    {
        ++lineNumber;
        if(line.empty())
            continue;

        std::istringstream row(line);
        Task task;
        int done = 0;
        int priority = 0;
        std::string extra;

        if(!(row >> task.id >> done >> priority
                 >> std::quoted(task.title)
                 >> std::quoted(task.description)
                 >> std::quoted(task.category)
                 >> std::quoted(task.dueDate)) ||
           (row >> extra) || task.id <= 0 || ids.count(task.id) ||
           (done != 0 && done != 1) || priority < 1 || priority > 3 ||
           task.title.empty() || task.title.size() > 120 ||
           task.description.size() > 500 || task.category.empty() ||
           task.category.size() > 40 || !validDate(task.dueDate) ||
           task.title.find_first_of("\r\n") != std::string::npos ||
           task.description.find_first_of("\r\n") != std::string::npos ||
           task.category.find_first_of("\r\n") != std::string::npos)
        {
            error = "Invalid task data on line " + std::to_string(lineNumber) + ".";
            tasks.clear();
            return false;
        }

        task.completed = done == 1;
        task.priority = static_cast<Priority>(priority);
        tasks.push_back(task);
        ids.insert(task.id);
    }

    if(input.bad())
    {
        error = "Error while reading the task data file.";
        tasks.clear();
        return false;
    }
    return true;
}

bool TaskRepository::save(const std::vector<Task>& tasks, std::string& error) const
{
    error.clear();
    std::error_code ec;
    const auto parent = filePath_.parent_path();
    if(!parent.empty())
    {
        std::filesystem::create_directories(parent, ec);
        if(ec)
        {
            error = "Cannot create the data folder: " + ec.message();
            return false;
        }
    }

    const auto tempFile = std::filesystem::path(filePath_.string() + ".tmp");
    const auto backupFile = std::filesystem::path(filePath_.string() + ".bak");
    {
        std::ofstream output(tempFile, std::ios::trunc);
        if(!output)
        {
            error = "Cannot create temporary task data file.";
            return false;
        }

        output << "TASKS_V1\n";
        for(const Task& task : tasks)
        {
            output << task.id << ' ' << (task.completed ? 1 : 0) << ' '
                   << static_cast<int>(task.priority) << ' '
                   << std::quoted(task.title) << ' '
                   << std::quoted(task.description) << ' '
                   << std::quoted(task.category) << ' '
                   << std::quoted(task.dueDate) << '\n';
        }
        output.flush();
        if(!output)
        {
            error = "Error writing task data.";
            return false;
        }
    }

    const bool hadOldFile = std::filesystem::exists(filePath_, ec);
    if(ec)
    {
        error = "Cannot check existing data: " + ec.message();
        return false;
    }

    if(hadOldFile)
    {
        std::filesystem::remove(backupFile, ec);
        if(ec)
        {
            error = "Cannot replace the previous data backup: " + ec.message();
            return false;
        }
        std::filesystem::rename(filePath_, backupFile, ec);
        if(ec)
        {
            error = "Cannot back up previous task data: " + ec.message();
            return false;
        }
    }

    std::filesystem::rename(tempFile, filePath_, ec);
    if(ec)
    {
        if(hadOldFile)
        {
            std::error_code restoreError;
            std::filesystem::rename(backupFile, filePath_, restoreError);
            if(restoreError)
                error = "Could not save data or restore the backup. Backup: " + backupFile.string();
            else
                error = "Could not save tasks; previous data was restored: " + ec.message();
        }
        else
        {
            error = "Could not save tasks: " + ec.message();
        }
        return false;
    }

    if(hadOldFile)
    {
        std::error_code cleanupError;
        std::filesystem::remove(backupFile, cleanupError);
    }
    return true;
}
