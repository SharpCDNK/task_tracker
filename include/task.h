#pragma once

#include <string>

struct Task {
    int id;
    std::string description;
    std::string status;
    std::string createdAt;
    std::string updatedAt;
};
