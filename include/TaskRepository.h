#pragma once

#include "Task.h"
#include <filesystem>
#include <string>
#include <vector>

class TaskRepository
{
public:
    explicit TaskRepository(std::filesystem::path filePath);

    bool load(std::vector<Task>& tasks, std::string& error) const;
    bool save(const std::vector<Task>& tasks, std::string& error) const;

private:
    std::filesystem::path filePath_;
};
