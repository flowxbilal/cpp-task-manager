#pragma once

#include "TaskRepository.h"

#include <string>
#include <vector>

class TaskManager
{
public:
    explicit TaskManager(TaskRepository repository);

    bool initialize(std::string& error);
    bool add(Task task, std::string& error);
    bool edit(int id, const Task& changes, std::string& error);
    bool setCompleted(int id, bool completed, std::string& error);
    bool remove(int id, std::string& error);

    const Task* find(int id) const;
    const std::vector<Task>& all() const;
    std::vector<Task> search(const std::string& query) const;
    std::vector<Task> byStatus(bool completed) const;

private:
    bool saveOrRestore(const std::vector<Task>& previous, std::string& error);
    bool validate(const Task& task, std::string& error) const;

    TaskRepository repository_;
    std::vector<Task> tasks_;
    int nextId_ = 1;
};
