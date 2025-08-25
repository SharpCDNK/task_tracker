#include <iostream>
#include <optional>
#include "../include/cli.h"
#include "../include/json.h"
#include "../include/task.h"


const std::string file_name = "../data/config.json";
    std::vector<Task> data = parseJsonToVector(file_name);

struct TaskStore{
    void addNewTask(std::string text){

    }

    void updateTask(int id, const std::string text){

    }

    void deleteTask(int id){

    }

    void markInProgress(int id){

    }

    void markDone(int id){

    }

    void listAll(){

    }

    void listByStatus(Status s){

    }
};



int main(int args, char* argv[]){
    std::optional<CommandInstance> parsed = parseArguments(args, argv);

    if(!parsed){
        return 1;
    }

    TaskStore store;
    auto& cmd = *parsed;

    if(cmd.name  == "update"){
        int id = std::get<int>(cmd.args[0]);
        std::string& text = std::get<std::string>(cmd.args[1]);
        store.updateTask(id,text);
    }else if(cmd.name == "add"){
        std::string& text = std::get<std::string>(cmd.args[0]);
        store.addNewTask(text);
    }else if(cmd.name == "delete"){
        int id = std::get<int>(cmd.args[0]);
        store.deleteTask(id);
    }else if (cmd.name == "mark-in-progress"){
        int id = std::get<int>(cmd.args[0]);
        store.markInProgress(id);
    }else if (cmd.name == "mark-done"){
        int id = std::get<int>(cmd.args[0]);
        store.markDone(id);
    } else if (cmd.name == "list"){
        if(cmd.args.empty()){
            store.listAll();
        }else{
            Status cur = std::get<Status>(cmd.args[0]);
            store.listByStatus(cur);
        }
    }

    return 1;
}
