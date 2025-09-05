#include "../include/cli.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>


std::string toString(Status s){
    switch(s){
        case Status::ToDo:          return "todo";
        case Status::InProgress:    return "inprogress";
        case Status::Done:          return "done";
    }

    return "todo";
}

static std::string trim(std::string_view s){
    size_t b = 0, e = s.size() - 1;

    while(b < e && std::isspace(static_cast<unsigned char>(s[b]))){
        b++;
    }
    while(e > b && std::isspace(static_cast<unsigned char>(s[e]))){
        e--;
    }

    return std::string(s.substr(b, e - b + 1));
}

static std::string toLower(std::string s){
    std::transform(s.begin(),s.end(), s.begin(),
            [](unsigned char c){return static_cast<unsigned char>(std::tolower(c));});

    return s;
}

static std::optional<int> parseIntSafe(std::string_view sv){
    int value = 0;
    std::from_chars_result res = std::from_chars(sv.data(), sv.data() + sv.size(), value, 10);

    if(res.ec != std::errc() || res.ptr != sv.data() + sv.size()){
        return std::nullopt;
    }

    return value >= 0 ? std::optional<int>{value} : std::nullopt;
}

static std::optional<Status> parseStatus(std::string_view sv){
    std::string s = toLower(trim(sv));
    if(s == "todo"){
        return Status::ToDo;
    }
    if(s == "inprogress" || s == "in-progress"){
        return Status::InProgress;
    }
    if(s == "done"){
        return Status::Done;
    }

    return std::nullopt;
}

const std::vector<CommandSpec>& commandRegistry(){
    static const std::vector<CommandSpec> registry = {
        {   "update",           {ArgType::Int, ArgType::StringRest},    "task-cli update <id:int> <text>",      "\t\t\t\tUpdate a task"},
        {   "delete",           {ArgType::Int},                         "task-cli delete <id:int>",             "\t\t\t\tDelete a task"},
        {   "add",              {ArgType::StringRest},                  "task-cli add <text>",                  "\t\t\t\t\tAdd a new task"},
        {   "mark-in-progress", {ArgType::Int},                         "task-cli mark-in-progress <id:int>",   "\t\t\tMark a task as in progress"},
        {   "mark-done",        {ArgType::Int},                         "task-cli mark-done <id:int>",          "\t\t\t\tMark a task as done"},
        {   "list",             {},                                     "task-cli list",                        "\t\t\t\t\t\tList of all tasks"},
        {   "list",             {ArgType::Status},                      "task-cli list <status>",               "\t\t\t\t\tShow tasks by status"},
    };

    return registry;
}

void printAllCommandsUsage(std::ostream& os){
    os << "Available commands:\n";

    for(const auto& c: commandRegistry()){
        os << c.usage ;
        if(!c.description.empty()){
            os <<  c.description;
        }
        os << "\n";
    }
}

static const CommandSpec* selectSpec(const std::string& name, int argcRemaining){
    for(const auto& spec: commandRegistry()){
        if(spec.name != name){
            continue;
        }

        const bool lastIsRest = !spec.argTypes.empty() && spec.argTypes.back() == ArgType::StringRest;
        const int required = static_cast<int>(spec.argTypes.size());
        const bool ok = lastIsRest ? (argcRemaining >= required) : (argcRemaining == required);

        if(ok){
            return &spec;
        }
    }

    return nullptr;
}

std::optional<CommandInstance> parseArguments(int argc, char* argv[]){

    if(argc < 2){
       std::cerr << "Unknow command\n";
       printAllCommandsUsage(std::cout);
       return std::nullopt;

    }

    std::string command = toLower(argv[1]);
    int remaining = argc - 2;

    const CommandSpec* spec = selectSpec(command, remaining);

    if(!spec){
        std::cerr << "Unknow command or wrong arguments\n";
        printAllCommandsUsage(std::cout);
        return std::nullopt;
    }

    std::vector<ArgValue> parsed;
    parsed.reserve(spec->argTypes.size());

    int pos = 2;

    for(size_t i = 0; i < spec->argTypes.size(); i++){
        ArgType t = spec->argTypes[i];
        switch(t){
            case ArgType::Int: {
                if(pos >= argc || !parseIntSafe(argv[pos])){
                    std::cerr << "Waiting for <id:int> on position " << i + 1 << ". Usage: " << spec->usage << '\n';
                    return std::nullopt;
                }
                parsed.emplace_back(*parseIntSafe(argv[pos]));
                pos++;
                break;
            }
            case ArgType::Status: {
                if(pos >= argc || !parseStatus(argv[pos])){
                    std::cerr << "Waiting for status done|todo|inprogress on position " << i + 1 << ". Usage: " << spec->usage << '\n';
                    return std::nullopt;
                }
                parsed.emplace_back(*parseStatus(argv[pos]));
                pos++;
                break;
            }
            case ArgType::StringRest: {
                std::ostringstream oss;
                for(size_t j = pos; j < argc; j++){
                    if(j > pos){
                        oss << ' ';
                    }
                    oss << argv[j];
                }

                std::string text = oss.str();

                if(text.empty()){
                    std::cerr << "Waitin for <text>, text shouldn't be empty. " << "Usage: " << spec->usage << '\n';
                    return std::nullopt;
                }
                parsed.emplace_back(std::move(text));
                pos = argc;
                break;
            }
        }

    }

    if(pos < argc){
        std::cerr << "Too many arguments. " << "Usage:" << spec->usage << '\n';
        return std::nullopt;
    }

    return CommandInstance{command, std::move(parsed), spec};
}
