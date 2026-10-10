#pragma once

#include <string>

enum class Priority
{
    Low = 1,
    Medium = 2,
    High = 3
};

struct Task
{
    int id = 0;
    std::string title;
    std::string description;
    std::string category = "General";
    Priority priority = Priority::Medium;
    std::string dueDate;
    bool completed = false;
};

std::string priorityName(Priority priority);
bool validDate(const std::string& date);
std::string trim(const std::string& text);
