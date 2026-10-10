#include "Task.h"

#include <cctype>

std::string priorityName(Priority priority)
{
    switch(priority)
    {
        case Priority::Low: return "Low";
        case Priority::Medium: return "Medium";
        case Priority::High: return "High";
    }
    return "Unknown";
}

std::string trim(const std::string& text)
{
    const auto first = text.find_first_not_of(" \t\r\n");
    if(first == std::string::npos)
        return "";

    const auto last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

bool validDate(const std::string& date)
{
    if(date.empty())
        return true;

    if(date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;

    for(int i = 0; i < 10; i++)
    {
        if(i == 4 || i == 7)
            continue;
        if(!std::isdigit(static_cast<unsigned char>(date[i])))
            return false;
    }

    const int year = std::stoi(date.substr(0, 4));
    const int month = std::stoi(date.substr(5, 2));
    const int day = std::stoi(date.substr(8, 2));
    if(year < 2000 || year > 9999 || month < 1 || month > 12 || day < 1)
        return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const bool leapYear = year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
    if(leapYear)
        daysInMonth[1] = 29;

    return day <= daysInMonth[month - 1];
}
