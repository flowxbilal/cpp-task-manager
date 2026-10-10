#include "TaskManager.h"

#include <algorithm>
#include <cctype>
#include <climits>
#include <utility>

namespace
{
    std::string lowercase(std::string text)
    {
        for(char& c : text)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return text;
    }
}

TaskManager::TaskManager(TaskRepository repository)
    : repository_(std::move(repository))
{
}

bool TaskManager::initialize(std::string& error)
{
    if(!repository_.load(tasks_, error))
        return false;

    nextId_ = 1;
    for(const Task& task : tasks_)
    {
        if(task.id == INT_MAX)
        {
            error = "Task ID limit reached.";
            return false;
        }
        nextId_ = std::max(nextId_, task.id + 1);
    }
    return true;
}

bool TaskManager::validate(const Task& task, std::string& error) const
{
    if(task.title.empty() || task.title.size() > 120)
        error = "Title must contain 1 to 120 characters.";
    else if(task.description.size() > 500)
        error = "Description cannot exceed 500 characters.";
    else if(task.category.empty() || task.category.size() > 40)
        error = "Category must contain 1 to 40 characters.";
    else if(!validDate(task.dueDate))
        error = "Due date must be a valid date in YYYY-MM-DD format.";
    else if(task.priority < Priority::Low || task.priority > Priority::High)
        error = "Invalid priority.";
    else if(task.title.find_first_of("\r\n") != std::string::npos ||
            task.description.find_first_of("\r\n") != std::string::npos ||
            task.category.find_first_of("\r\n") != std::string::npos)
        error = "Task fields cannot contain line breaks.";
    else
        return true;

    return false;
}

bool TaskManager::saveOrRestore(const std::vector<Task>& previous, std::string& error)
{
    if(repository_.save(tasks_, error))
        return true;

    tasks_ = previous;
    return false;
}

bool TaskManager::add(Task task, std::string& error)
{
    task.title = trim(task.title);
    task.description = trim(task.description);
    task.category = trim(task.category);
    task.dueDate = trim(task.dueDate);
    if(!validate(task, error))
        return false;
    if(nextId_ == INT_MAX)
    {
        error = "Task ID limit reached.";
        return false;
    }

    const auto previous = tasks_;
    task.id = nextId_;
    tasks_.push_back(std::move(task));
    if(!saveOrRestore(previous, error))
        return false;

    ++nextId_;
    return true;
}

bool TaskManager::edit(int id, const Task& changes, std::string& error)
{
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
                           [id](const Task& task) { return task.id == id; });
    if(it == tasks_.end())
    {
        error = "Task not found.";
        return false;
    }

    Task updated = changes;
    updated.id = id;
    updated.completed = it->completed;
    updated.title = trim(updated.title);
    updated.description = trim(updated.description);
    updated.category = trim(updated.category);
    updated.dueDate = trim(updated.dueDate);
    if(!validate(updated, error))
        return false;

    const auto previous = tasks_;
    *it = std::move(updated);
    return saveOrRestore(previous, error);
}

bool TaskManager::setCompleted(int id, bool completed, std::string& error)
{
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
                           [id](const Task& task) { return task.id == id; });
    if(it == tasks_.end())
    {
        error = "Task not found.";
        return false;
    }

    const auto previous = tasks_;
    it->completed = completed;
    return saveOrRestore(previous, error);
}

bool TaskManager::remove(int id, std::string& error)
{
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
                           [id](const Task& task) { return task.id == id; });
    if(it == tasks_.end())
    {
        error = "Task not found.";
        return false;
    }

    const auto previous = tasks_;
    tasks_.erase(it);
    return saveOrRestore(previous, error);
}

const Task* TaskManager::find(int id) const
{
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
                           [id](const Task& task) { return task.id == id; });
    return it == tasks_.end() ? nullptr : &*it;
}

const std::vector<Task>& TaskManager::all() const
{
    return tasks_;
}

std::vector<Task> TaskManager::search(const std::string& query) const
{
    std::vector<Task> results;
    const std::string needle = lowercase(trim(query));
    for(const Task& task : tasks_)
    {
        if(lowercase(task.title).find(needle) != std::string::npos ||
           lowercase(task.description).find(needle) != std::string::npos ||
           lowercase(task.category).find(needle) != std::string::npos)
            results.push_back(task);
    }
    return results;
}

std::vector<Task> TaskManager::byStatus(bool completed) const
{
    std::vector<Task> results;
    for(const Task& task : tasks_)
    {
        if(task.completed == completed)
            results.push_back(task);
    }
    return results;
}
