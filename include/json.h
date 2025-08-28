#pragma once

#include <string>
#include <vector>
#include "../include/task.h"

std::string readFileToString(const std::string& file_name);
void writeStringToFile(const std::string& file_name, const std::string& content);

std::vector<Task> parseJsonToVector(const std::string& file_name);
std::string getContentFromVector(const std::vector<Task>& tasks);
