#include <iostream>
#include <string>
#include <sstream>
#include "KeyValueStore.h" 

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
            break;
        }
        std::stringstream ss(line);
        std::string command;
        ss >> command;
        if (command == "EXIT") {
            kvs.save(FILENAME);
            std::cout << "Data saved in data.csv" << std::endl;
            break;
        }
        else if (command == "BEGIN") {
            kvs.begin();
        } 
        else if (command == "COMMIT") {
            kvs.commit();
        } 
        else if (command == "ROLLBACK") {
            kvs.rollback();
        }
        else if (command == "SET") {
            std::string key, value;
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
                    std::cout << "(nil)" << std::endl;
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