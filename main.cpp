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

    bool save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    for (const auto& pair : data) {
        // Check if the value contains a comma
        if (pair.second.find(',') != std::string::npos) {
            // If it does, enclose the value in double quotes
            file << pair.first << ",\"" << pair.second << "\"\n";
        } else {
            // Otherwise, write it normally
            file << pair.first << "," << pair.second << "\n";
        }
    }

    file.close();
    return true;
}

    bool load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return true;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::string key, value;
        size_t comma_pos = line.find(',');
        if (comma_pos == std::string::npos) {
            continue; // Skip malformed lines
        }

        key = line.substr(0, comma_pos);
        std::string value_part = line.substr(comma_pos + 1);

        // Check if the value part is quoted
        if (!value_part.empty() && value_part.front() == '"' && value_part.back() == '"') {
            // If so, strip the quotes
            value = value_part.substr(1, value_part.length() - 2);
        } else {
            value = value_part;
        }
        
        data[key] = value;
    }
    file.close();
    return true;
}

    size_t count() const {
        return data.size();
    }

};

int main() {
    KeyValueStore kvs;
    std::string line;
    const std::string FILENAME = "data.csv";
    kvs.load(FILENAME);

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
        else if (command == "COUNT") {
            std::cout << kvs.count() << std::endl;
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