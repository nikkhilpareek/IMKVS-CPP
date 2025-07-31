#include "KeyValueStore.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "json.hpp"
#include <chrono>

using json = nlohmann::json;

void to_json(json& j, const ValueWithTTL& v) {
    j = json{{"value", v.value}, {"expiration_time_ms", v.expiration_time_ms}};
}

void from_json(const json& j, ValueWithTTL& v) {
    j.at("value").get_to(v.value);
    j.at("expiration_time_ms").get_to(v.expiration_time_ms);
}



long long getCurrentTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

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

void KeyValueStore::set(const std::string& key, const std::string& value, long long ttl_ms) {
    long long expiration_time = -1;
    if (ttl_ms > 0) {
        expiration_time = getCurrentTimeMillis() + ttl_ms;
    }

    ValueWithTTL entry = {value, expiration_time};

    if (in_trxn) {
        trxn_data[key] = entry;
    } else {
        data[key] = entry;
    }
}

std::optional<std::string> KeyValueStore::get(const std::string& key) {
    if (in_trxn) {
        auto it = trxn_data.find(key);
        if (it != trxn_data.end()) {
            if (!it->second.has_value()) {
                return std::nullopt;
            }
            return it->second->value;
        }
    }

    auto it = data.find(key);
    if (it != data.end()) {
        if (it->second.is_expired()) {
            data.erase(it);
            return std::nullopt;
        }
        return it->second.value;
    }
    return std::nullopt;
}

bool KeyValueStore::remove(const std::string& key) {
    if (in_trxn) {
        trxn_data[key] = std::nullopt;
        return true;
    }
    get(key);
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

    // Create a temporary map to hold only non-expired items
    std::unordered_map<std::string, ValueWithTTL> non_expired_data;
    for (const auto& pair : data) {
        if (!pair.second.is_expired()) {
            non_expired_data[pair.first] = pair.second;
        }
    }

    json j = non_expired_data;
    file << j.dump(4);
    file.close();
    return true;
}

bool KeyValueStore::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return true; // File doesn't exist yet, which is fine on first run.
    }

    try {
        json j;
        file >> j;
        data = j.get<std::unordered_map<std::string, ValueWithTTL>>();
    } catch (const json::parse_error& e) {
        std::cerr << "ERROR: Failed to parse JSON file: " << e.what() << std::endl;
        return false;
    }
    
    file.close();
    return true;
}