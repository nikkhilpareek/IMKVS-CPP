#include "KeyValueStore.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

using json = nlohmann::json;

void to_json(json& j, const ValueWithTTL& v) {
    j = { {"expiration_time_ms", v.expiration_time_ms} };
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j["type"] = "string";
            j["data"] = arg; // <-- FIX: Changed "value" to "data"
        } else if constexpr (std::is_same_v<T, long long>) {
            j["type"] = "integer";
            j["data"] = arg; // <-- FIX: Changed "value" to "data"
        }
    }, v.data);
}

void from_json(const json& j, ValueWithTTL& v) {
    j.at("expiration_time_ms").get_to(v.expiration_time_ms);
    std::string type = j.at("type").get<std::string>();
    if (type == "string") {
        v.data = j.at("data").get<std::string>(); // <-- FIX: Changed "value" to "data"
    } else if (type == "integer") {
        v.data = j.at("data").get<long long>(); // <-- FIX: Changed "value" to "data"
    }
}

long long getCurrentTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
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
            if (!it->second.has_value()) return std::nullopt;
            return std::visit([](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, long long>) {
                    return std::to_string(arg);
                }
            }, it->second->data);
        }
    }
    auto it = data.find(key);
    if (it != data.end()) {
        if (it->second.is_expired()) {
            data.erase(it);
            return std::nullopt;
        }
        return std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, long long>) {
                return std::to_string(arg);
            }
        }, it->second.data);
    }
    return std::nullopt;
}


std::optional<long long> perform_op(std::optional<ValueWithTTL>& entry, const std::string& op) {
    if (!entry.has_value() || entry->is_expired()) {
        long long start_val = (op == "INCR") ? 1LL : -1LL;
        entry = {start_val, -1};
        return start_val;
    }

    long long new_value;
    bool success = false;
    std::visit([&](auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, long long>) {
            new_value = (op == "INCR") ? ++arg : --arg;
            success = true;
        } else if constexpr (std::is_same_v<T, std::string>) {
            try {
                long long val = std::stoll(arg);
                new_value = (op == "INCR") ? ++val : --val;
                entry->data = new_value; 
                success = true;
            } catch (...) {
                success = false;
            }
        }
    }, entry->data);

    if (success) {
        return new_value;
    }
    return std::nullopt;
}


std::optional<long long> KeyValueStore::incr(const std::string& key) {
    if (in_trxn) {
        std::optional<ValueWithTTL> current_val = std::nullopt;
        
        if (trxn_data.count(key)) {
            current_val = trxn_data.at(key);
        } else if (data.count(key)) {
            current_val = data.at(key);
        }
        auto result = perform_op(current_val, "INCR");
        if (result.has_value()) {
            trxn_data[key] = current_val; 
        }
        return result;
    }
    
    std::optional<ValueWithTTL> entry = std::nullopt;
    if (data.count(key)) {
        entry = data.at(key);
    }
    auto result = perform_op(entry, "INCR");
    if (result.has_value()) {
        data[key] = entry.value(); 
    }
    return result;
}

std::optional<long long> KeyValueStore::decr(const std::string& key) {
    if (in_trxn) {
        std::optional<ValueWithTTL> current_val = std::nullopt;
        if (trxn_data.count(key)) {
            current_val = trxn_data.at(key);
        } else if (data.count(key)) {
            current_val = data.at(key);
        }
        auto result = perform_op(current_val, "DECR");
        if (result.has_value()) {
            trxn_data[key] = current_val;
        }
        return result;
    }

    std::optional<ValueWithTTL> entry = std::nullopt;
    if (data.count(key)) {
        entry = data.at(key);
    }
    auto result = perform_op(entry, "DECR");
    if (result.has_value()) {
        data[key] = entry.value();
    }
    return result;
}

bool KeyValueStore::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file for writing: " << filename << std::endl;
        return false;
    }
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
    if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
        return true; // File doesn't exist or is empty, which is fine.
    }

    json j;
    try {
        file >> j; // Parse the entire file into a JSON object first.
    } catch (const json::parse_error& e) {
        // This is a major syntax error. The file is unreadable.
        std::cerr << "[ERROR] Failed to parse " << filename << ". It is not valid JSON." << std::endl;
        std::cerr << "        Details: " << e.what() << std::endl;
        std::cerr << "        Starting with an empty database." << std::endl;
        data.clear();
        return true;
    }

    // Now, iterate and load each entry individually.
    int corrupted_entries = 0;
    for (auto& element : j.items()) {
        const std::string& key = element.key();
        try {
            // Try to deserialize this specific entry.
            ValueWithTTL value = element.value().get<ValueWithTTL>();
            data[key] = value;
        } catch (const json::exception& e) {
            // This specific entry is malformed, but others might be okay.
            std::cerr << "[WARNING] Skipping corrupted data for key '" << key << "'. Details: " << e.what() << std::endl;
            corrupted_entries++;
        }
    }

    if (corrupted_entries > 0) {
        std::cerr << "[INFO] Successfully loaded " << data.size() << " entries, skipped " << corrupted_entries << " corrupted entries." << std::endl;
    }
    
    file.close();
    return true;
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