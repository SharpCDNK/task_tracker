#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../include/cli.h"
#include "../include/json.h"
#include "../include/task.h"

std::vector<Task> data;
std::string g_state_path;

std::string getStateFilePath() {
    const char* home = std::getenv("HOME");
#ifdef _WIN32
    if (!home) {
        home = std::getenv("USERPROFILE");
    }
#endif
    if (!home) {
        throw std::runtime_error("Unable to determine the home directory");
    }
    std::filesystem::path p = std::filesystem::path(home) / ".todo_cli" / "state.json";
    return p.string();
}

void ensureStateFile(const std::string& path) {
    std::filesystem::path p(path);
    std::filesystem::create_directories(p.parent_path());
    if (!std::filesystem::exists(p)) {
        std::ofstream ofs(path);
        ofs << "[\n]";
    }
}

std::string getCurTime() {
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

struct TaskStore {
    void addNewTask(const std::string& text) {
        Task _new;
        _new.description = text;
        _new.createdAt = getCurTime();
        _new.updatedAt = _new.createdAt;
        _new.status = toString(Status::ToDo);

        int newid = 0;
        for (const auto& i : data) {
            if (i.id > newid) {
                newid = i.id;
            }
        }
        _new.id = newid + 1;
        data.emplace_back(_new);
    }

    void updateTask(int id, const std::string& text) {
        auto it = std::find_if(data.begin(), data.end(),
                               [id](const Task& t) { return t.id == id; });

        if (it == data.end()) {
            throw std::runtime_error("Task with id = " + std::to_string(id) + " not found.");
        }

        it->description = text;
        it->updatedAt = getCurTime();
    }

    void deleteTask(int id) {
        auto it = std::find_if(data.begin(), data.end(),
                               [id](const Task& t) { return t.id == id; });

        if (it == data.end()) {
            throw std::runtime_error("Task with id = " + std::to_string(id) + " not found.");
        }

        data.erase(it);
    }

    void markInProgress(int id) {
        auto it = std::find_if(data.begin(), data.end(),
                               [id](const Task& t) { return t.id == id; });

        if (it == data.end()) {
            throw std::runtime_error("Task with id = " + std::to_string(id) + " not found.");
        }

        it->updatedAt = getCurTime();
        it->status = toString(Status::InProgress);
    }

    void markDone(int id) {
        auto it = std::find_if(data.begin(), data.end(),
                               [id](const Task& t) { return t.id == id; });

        if (it == data.end()) {
            throw std::runtime_error("Task with id = " + std::to_string(id) + " not found.");
        }

        it->updatedAt = getCurTime();
        it->status = toString(Status::Done);
    }

    void listAll() const {
        if (data.empty()) {
            std::cout << "No data in file " << g_state_path << '\n';
            return;
        }

        for (const auto& item : data) {
            std::cout << "Task № " << item.id << '\n';
            std::cout << "Description: " << item.description << '\n';
            std::cout << "Status: " << item.status << '\n';
            std::cout << "UpdatedAt: " << item.updatedAt << '\n';
            std::cout << "CreatedAt: " << item.createdAt << "\n\n";
        }
    }

    void listByStatus(Status s) const {
        if (data.empty()) {
            std::cout << "No data in file " << g_state_path << '\n';
            return;
        }

        int count = 0;
        for (const auto& item : data) {
            if (item.status == toString(s)) {
                count++;
                std::cout << "Task № " << item.id << '\n';
                std::cout << "Description: " << item.description << '\n';
                std::cout << "Status: " << item.status << '\n';
                std::cout << "UpdatedAt: " << item.updatedAt << '\n';
                std::cout << "CreatedAt: " << item.createdAt << "\n\n";
            }
        }
        if (count == 0) {
            std::cout << "No data with status " << toString(s) << '\n';
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        g_state_path = getStateFilePath();
        ensureStateFile(g_state_path);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    // Читаем состояние из файла
    data = parseJsonToVector(g_state_path);

    std::optional<CommandInstance> parsed = parseArguments(argc, argv);
    if (!parsed) {
        return 1;
    }

    TaskStore store;
    auto& cmd = *parsed;

    if (cmd.name == "update") {
        int id = std::get<int>(cmd.args[0]);
        const std::string& text = std::get<std::string>(cmd.args[1]);
        store.updateTask(id, text);
    } else if (cmd.name == "add") {
        const std::string& text = std::get<std::string>(cmd.args[0]);
        store.addNewTask(text);
    } else if (cmd.name == "delete") {
        int id = std::get<int>(cmd.args[0]);
        store.deleteTask(id);
    } else if (cmd.name == "mark-in-progress") {
        int id = std::get<int>(cmd.args[0]);
        store.markInProgress(id);
    } else if (cmd.name == "mark-done") {
        int id = std::get<int>(cmd.args[0]);
        store.markDone(id);
    } else if (cmd.name == "list") {
        if (cmd.args.empty()) {
            store.listAll();
        } else {
            Status cur = std::get<Status>(cmd.args[0]);
            store.listByStatus(cur);
        }
    }

    writeStringToFile(g_state_path, getContentFromVector(data));
    return 0;
}

