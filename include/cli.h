#pragma once

#include <string>
#include <variant>
#include <vector>
#include <optional>
#include <ostream>


enum class Status { ToDo, InProgress, Done };

std::string toString(Status s);

enum class ArgType {
    Int,
    StringRest,
    Status
};

using ArgValue = std::variant<int, std::string, Status>;

struct CommandSpec {
    std::string name;
    std::vector<ArgType> argTypes;
    std::string usage;
    std::string description;
};

struct CommandInstance {
    std::string name;
    std::vector<ArgValue> args;
    const CommandSpec* spec;
};

const std::vector<CommandSpec>& commandRegistry();
void printAllCommandsUsage(std::ostream& os);
std::optional<CommandInstance> parseArguments(int argc, char* argv[]);
