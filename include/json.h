#pragma once

#include <string>
#include <vector>
#include "../include/task.h"

std::string readFileToString(const std::string& file_name);
void writeStringToFile(const std::string& file_name, const std::string& content);

std::string jsonEscape(const std::string& s);
std::string jsonUnescape(const std::string& s);
std::string extractRaw(const std::string& json, const std::string& key);

std::string toJson(const Task& t);
Task parseJsonObject(const std::string& json);

std::vector<Task> parseJsonToVector(const std::string& file_name);
std::string getContentFromVector(const std::vector<Task>& tasks);
