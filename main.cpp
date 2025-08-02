#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "KeyValueStore.h"

void print_help() {
    std::cout << "IMKVS Help:\n"
              << "--------------------------------------------------------------------------\n"
              << "  SET key value [ttl_ms]  - Sets a key to a value with an optional TTL.\n"
              << "  GET key                 - Retrieves the value for a given key.\n"
              << "  REMOVE key              - Deletes a key-value pair.\n"
              << "  INCR key                - Atomically increments an integer key.\n"
              << "  DECR key                - Atomically decrements an integer key.\n"
              << "  COUNT                   - Returns the total number of keys.\n"
              << "--------------------------------------------------------------------------\n"
              << "  BEGIN                   - Starts a new transaction.\n"
              << "  COMMIT                  - Saves all changes in the current transaction.\n"
              << "  ROLLBACK                - Discards all changes in the current transaction.\n"
              << "--------------------------------------------------------------------------\n"
              << "  HELP                    - Shows this help message.\n"
              << "  EXIT                    - Saves the database and closes the CLI.\n"
              << "--------------------------------------------------------------------------\n";
}

int main() {
    KeyValueStore kvs;
    std::string line;
    const std::string FILENAME = std::string(PROJECT_SOURCE_DIR) + "/data.json";
    kvs.load(FILENAME);

    std::cout << "Nikhil's In-Memory Key-Value Store Project" << std::endl;
    std::cout << "Enter commands (e.g., SET, GET, INCR, DECR, EXIT)" << std::endl;
    std::cout << "Enter 'HELP' for a list of commands." << std::endl;
    
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
            std::cout << "Data saved to data.json" << std::endl;
            break;
        }
        else if (command == "HELP") {
            print_help();
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
            std::string key;
            if (!(ss >> key)) {
                std::cout << "ERROR: Incorrect usage. Try SET key value [ttl_ms]" << std::endl;
                continue;
            }

            std::string rest_of_line;
            std::getline(ss, rest_of_line);

            size_t first_char = rest_of_line.find_first_not_of(" \t");
            if (std::string::npos == first_char) {
                std::cout << "ERROR: Value cannot be empty for SET command." << std::endl;
                continue;
            }
            rest_of_line = rest_of_line.substr(first_char);

            long long ttl_ms = -1;
            std::string value = rest_of_line;

            size_t last_space = rest_of_line.find_last_of(" \t");
            if (last_space != std::string::npos) {
                std::string last_word = rest_of_line.substr(last_space + 1);
                try {
                    size_t pos;
                    long long potential_ttl = std::stoll(last_word, &pos);
                    if (pos == last_word.length()) {
                        ttl_ms = potential_ttl;
                        value = rest_of_line.substr(0, last_space);
                    }
                } catch (const std::invalid_argument&) {
                } catch (const std::out_of_range&) {
                }
            }
            kvs.set(key, value, ttl_ms);
            std::cout << "OK" << std::endl;
        }else if(command == "GET"){
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
        }

        else if (command == "INCR") {
            std::string key;
            if (ss >> key) {
                if (auto new_value = kvs.incr(key)) {
                    std::cout << "(integer) " << *new_value << std::endl;
                } else {
                    std::cout << "ERROR: Value is not an integer or out of range." << std::endl;
                }
            } else {
                std::cout << "ERROR: Incorrect usage. Try INCR key" << std::endl;
            }
        }
        else if (command == "DECR") {
            std::string key;
            if (ss >> key) {
                if (auto new_value = kvs.decr(key)) {
                    std::cout << "(integer) " << *new_value << std::endl;
                } else {
                    std::cout << "ERROR: Value is not an integer or out of range." << std::endl;
                }
            } else {
                std::cout << "ERROR: Incorrect usage. Try DECR key" << std::endl;
            }
        }

        else if (!command.empty()) {
            std::cout << "ERROR: Unknown command '" << command << "'" << std::endl;
        }
    }
    std::cout<<"EXITING.."<<std::endl;
    return 0;
}