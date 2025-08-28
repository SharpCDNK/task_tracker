#include <cctype>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/json.h"

std::string readFileToString(const std::string& file_name){
    std::ifstream file(file_name);
    if(!file.is_open()){
        std::ofstream create_file(file_name);
        if(!create_file.is_open()){
            throw std::runtime_error("Couldn't create the file " + file_name);
        }
        create_file << "[\n]";
        create_file.close();
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();

    std::string content = ss.str();
    return content;
}


void writeStringToFile(const std::string& file_name, const std::string& content){
    std::ofstream file(file_name);

    if(!file.is_open()){
        throw std::runtime_error("Couldn't open file " + file_name);
    }

    file << content;
    std::cout << "File " << file_name << " has been updated successfully\n";
    return;
}



static std::string jsonEscape(const std::string& s) {
    std::string out;
    for(const char& c: s){
        switch(c){
            case '\"':  out += "\\\"";  break;
            case '\\':  out += "\\\\";  break;
            case '\n':  out += "\\n";   break;
            default:
                        out += c;
        }
    }

    return out;
}


static std::string jsonUnescape(const std::string& s){
    std::string out;

    for(size_t i = 0; i < s.size(); i++){

        if(s[i] == '\\' && i + 1 < s.size()){
            const char next = s[i+1];
            switch(next){
                case '\"':      out += "\"";    break;
                case '\\':      out += "\\";    break;
                case 'n':       out += "\n";    break;
                default:
                                out += next;
            }
            i++;
        }else{
            out += s[i];
        }
    }

    return out;

}


static std::string toJson(const Task& t){
    std::string s = "  {\n";
    s += "    \"id\": " + std::to_string(t.id) + ",\n";
    s += "    \"description\": \"" + jsonEscape(t.description) + "\",\n";
    s += "    \"status\": \"" + jsonEscape(t.status) + "\",\n";
    s += "    \"createdAt\": \"" + jsonEscape(t.createdAt) + "\",\n";
    s += "    \"updatedAt\": \"" + jsonEscape(t.updatedAt) + "\"\n";
    s += "  }";

    return s;
}


static std::string extractRaw (const std::string& json, const std::string& key){
    std::string needle = key;

    size_t pos = json.find(needle);
    if(pos == std::string::npos)    return "";



    pos = json.find(':', pos + needle.size());
    if(pos == std::string::npos)    return "";


    size_t i = pos + 1;

    while(i < json.size() && std::isspace(static_cast<unsigned char>(json[i]))){
        i++;
    }


    if(i >= json.size())    return "";


    if(json[i] == '"'){
        size_t start = i++;

        while(i < json.size()){
            if(json[i] == '\\'){
                i += 2;
                continue;
            }

            if(json[i] == '"'){
                i++;
                break;
            }

            i++;
        }
        return json.substr(start + 1, (i - start)-2);
    }

    size_t start = i;

    while(i < json.size() && json[i] != ',' && json[i] != '}' && !std::isspace(static_cast<unsigned char>(json[i]))){
        i++;
    }

    return json.substr(start,i - start);
}


static Task parseJsonObject(const std::string& json){
    Task object;
    std::string idStr = extractRaw(json,"id");

    if(idStr.empty()){
        throw std::invalid_argument("ID not found");
    }

    object.id = std::stoi(idStr);
    object.description = jsonUnescape(extractRaw(json,"description"));
    object.status = jsonUnescape(extractRaw(json,"status"));
    object.createdAt = jsonUnescape(extractRaw(json,"createdAt"));
    object.updatedAt = jsonUnescape(extractRaw(json,"updatedAt"));

    return object;
}


std::vector<Task> parseJsonToVector(const std::string& file_name){
    const std::string content = readFileToString(file_name);

    std::vector<Task> tasks;

    std::string cur_json_obj;

    size_t start = content.find('{'), pos = 0;
    while(start != std::string::npos){
        pos = start;

        while(pos < content.size()){
            pos++;
            if(content[pos] == '}'){
                break;
            }
        }

        cur_json_obj = content.substr(start,pos - start + 1);
        tasks.emplace_back(parseJsonObject(cur_json_obj));
        start = content.find('{', pos);
    }

    return tasks;

}


std::string getContentFromVector(const std::vector<Task>& tasks){
    std::string content = "[\n";

    for(size_t i = 0; i < tasks.size(); i++){
        content += toJson(tasks[i]);
        if(i != tasks.size() - 1){
            content += ',';
        }
        content += '\n';
    }

    content += ']';

    return content;
}
