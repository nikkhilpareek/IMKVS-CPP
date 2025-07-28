#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <optional>
#include <fstream>

class KeyValueStore {
private:
    std::unordered_map<std::string, std::string> data;

public:
    // Set a key-value pair.
    void set(const std::string& key, const std::string& value) {
        data[key] = value;
    }

    // Get a value by its key.
    std::optional<std::string> get(const std::string& key) const {
        if (auto it = data.find(key); it != data.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    // Remove a key-value pair and return true if successful.
    bool remove(const std::string& key) {
        return data.erase(key) > 0;
    }

    bool save(const std::string &filename){
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false; // Could not open the file
        }

        // Implementation of adding data in the file
        for(auto const& it: data){
            file << it.first << ","<<it.second<<"\n";
        }

        file.close();
        return true;
    }
};

int main() {
    KeyValueStore kvs;
    std::string line;
    const std::string FILENAME = "data.csv";

    std::cout << "Nikhil's In-Memory Key-Value Store Project" << std::endl;
    std::cout << "Enter commands (e.g., SET key value, GET key, EXIT)" << std::endl;
    while(true){
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break; // Exit if input stream closes (e.g., Ctrl+D)
        }

        std::stringstream ss(line);
        std::string command;
        ss >> command; // this will parse the first command in the input

        if (command == "EXIT") {
            kvs.save(FILENAME);
            std::cout << "Data saved in data.csv" << std::endl;
            break;
        }
        else if (command == "SET") {
            std::string key, value;
            // now we try to extract a key and a value.
            if (ss >> key && ss >> value) {
                kvs.set(key, value);
                std::cout << "OK" << std::endl;
            } else {
                std::cout << "ERROR: Incorrect usage. Try SET key value" << std::endl;
            }
        } else if(command == "GET"){
            std::string key;
            if (ss >> key) {
                if (auto value = kvs.get(key)) {
                    std::cout << *value << std::endl;
                } else {
                    std::cout << "(nil)" << std::endl; // (nil) is a common way to show 'not found'
                }
            } else {
                std::cout << "ERROR: Incorrect usage. Try GET key" << std::endl;
            }
        }
        else if (command == "REMOVE") {
            std::string key;
            if (ss >> key) {
                if (kvs.remove(key)) {
                    std::cout << "OK" << std::endl;
                } else {
                    std::cout << "Key not found" << std::endl;
                }
            } else {
                std::cout << "ERROR: Incorrect usage. Try REMOVE key" << std::endl;
            }
        } else if (!command.empty()) {
            std::cout << "ERROR: Unknown command '" << command << "'" << std::endl;
        }

    }
    std::cout<<"EXITING.."<<std::endl;
    return 0;
}