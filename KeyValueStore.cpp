#include "KeyValueStore.h" 
#include <fstream>
#include <sstream>
#include <iostream>

void KeyValueStore::set(const std::string& key, const std::string& value) {
    data[key] = value;
}

std::optional<std::string> KeyValueStore::get(const std::string& key) const {
    if (auto it = data.find(key); it != data.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool KeyValueStore::remove(const std::string& key) {
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
    for (const auto& pair : data) {
        if (pair.second.find(',') != std::string::npos) {
            file << pair.first << ",\"" << pair.second << "\"\n";
        } else {
            file << pair.first << "," << pair.second << "\n";
        }
    }
    file.close();
    return true;
}

bool KeyValueStore::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return true;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string key, value;
        size_t comma_pos = line.find(',');
        if (comma_pos == std::string::npos) continue;
        key = line.substr(0, comma_pos);
        std::string value_part = line.substr(comma_pos + 1);
        if (!value_part.empty() && value_part.front() == '"' && value_part.back() == '"') {
            value = value_part.substr(1, value_part.length() - 2);
        } else {
            value = value_part;
        }
        data[key] = value;
    }
    file.close();
    return true;
}