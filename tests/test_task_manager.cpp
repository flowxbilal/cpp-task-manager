#include "TaskManager.h"

#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <string>

int main()
{
    const auto file = std::filesystem::current_path() / "task_manager_test_data" / "tasks.db";
    std::filesystem::remove_all(file.parent_path());

    auto require = [](bool condition) {
        if(!condition)
            throw std::runtime_error("A Task Manager test failed.");
    };

    std::string error;
    TaskManager manager{TaskRepository(file)};
    require(manager.initialize(error));
    require(manager.all().empty());

    Task first;
    first.title = "  Study DSA  ";
    first.description = "Linked lists and stacks";
    first.category = "University";
    first.dueDate = "2028-02-29";
    first.priority = Priority::High;
    require(manager.add(first, error));
    require(manager.all().size() == 1);
    require(manager.all()[0].title == "Study DSA");
    require(manager.all()[0].id == 1);

    Task second;
    second.title = "Build a project";
    require(manager.add(second, error));
    require(manager.search("PROJECT").size() == 1);
    require(manager.search("University").size() == 1);
    require(manager.byStatus(false).size() == 2);

    require(manager.setCompleted(1, true, error));
    require(manager.byStatus(true).size() == 1);

    Task modified = *manager.find(2);
    modified.title = "Build a tested project";
    modified.dueDate = "2027-04-30";
    require(manager.edit(2, modified, error));
    require(manager.find(2)->title == "Build a tested project");

    Task invalid;
    invalid.title = "   ";
    require(!manager.add(invalid, error));
    invalid.title = "Something";
    invalid.dueDate = "2027-02-29";
    require(!manager.add(invalid, error));
    require(!validDate("2026-13-01"));
    require(validDate("2028-02-29"));

    TaskManager reopened{TaskRepository(file)};
    require(reopened.initialize(error));
    require(reopened.all().size() == 2);
    require(reopened.find(1)->completed);
    require(reopened.find(2)->title == "Build a tested project");
    require(reopened.remove(1, error));
    require(reopened.all().size() == 1);
    require(reopened.all()[0].id == 2);

    Task third;
    third.title = "Third task";
    require(reopened.add(third, error));
    require(reopened.all()[1].id == 3);

    std::filesystem::remove_all(file.parent_path());
    std::cout << "All Task Manager tests passed.\n";
    return 0;
}
