#include "KeyValueStore.h" 
#include <fstream>
#include <sstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

void KeyValueStore::begin(){
    if (in_trxn) {
        std::cout << "ERROR: Transaction already in progress." << std::endl;
        return;
    }
    in_trxn = true;
    trxn_data.clear();
    std::cout << "OK" << std::endl;
}


void KeyValueStore::commit() {
    if (!in_trxn) {
        std::cout << "ERROR: No transaction to commit." << std::endl;
        return;
    }
    // Apply changes from transaction_data to the main data map
    for (const auto& pair : trxn_data) {
        if (pair.second.has_value()) {
            data[pair.first] = *pair.second;
        } else {
            data.erase(pair.first);
        }
    }
    in_trxn = false;
    trxn_data.clear();
    std::cout << "OK" << std::endl;
}

void KeyValueStore::rollback() {
    if (!in_trxn) {
        std::cout << "ERROR: No transaction to rollback." << std::endl;
        return;
    }
    in_trxn = false;
    trxn_data.clear();
    std::cout << "OK" << std::endl;
}

void KeyValueStore::set(const std::string& key, const std::string& value) {
    if (in_trxn) {
        trxn_data[key] = value;
    } else {
        data[key] = value;
    }
}

std::optional<std::string> KeyValueStore::get(const std::string& key) const {
    if (in_trxn) {
        if (auto it = trxn_data.find(key); it != trxn_data.end()) {
            return it->second; // Returns the value or nullopt if deleted
        }
    }
    if (auto it = data.find(key); it != data.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool KeyValueStore::remove(const std::string& key) {
    if (in_trxn) {
        trxn_data[key] = std::nullopt; // Mark for deletion
        return true;
    }
    return data.erase(key) > 0;
}

size_t KeyValueStore::count() const {
    return data.size();
}

bool KeyValueStore::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    // Convert the entire map to a JSON object
    json j = data;

    // Write the JSON object to the file with pretty-printing (4-space indent)
    file << j.dump(4);

    file.close();
    return true;
}

bool KeyValueStore::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return true; // File doesn't exist yet, which is fine.
    }

    try {
        // Parse the file directly into a JSON object
        json j;
        file >> j;

        // Convert the JSON object back into the map
        data = j.get<std::unordered_map<std::string, std::string>>();
    } catch (const json::parse_error& e) {
        std::cerr << "ERROR: Failed to parse JSON file: " << e.what() << std::endl;
        // Decide if you want to exit or start with a clean slate
        return false;
    }
    
    file.close();
    return true;
}