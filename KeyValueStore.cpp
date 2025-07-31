#include "KeyValueStore.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include "picosha2.h"

using json = nlohmann::json;

void to_json(json& j, const ValueWithTTL& v) {
    j = { {"expiration_time_ms", v.expiration_time_ms} };
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j["type"] = "string";
            j["data"] = arg; 
        } else if constexpr (std::is_same_v<T, long long>) {
            j["type"] = "integer";
            j["data"] = arg; 
        }
    }, v.data);
}

void from_json(const json& j, ValueWithTTL& v) {
    j.at("expiration_time_ms").get_to(v.expiration_time_ms);
    std::string type = j.at("type").get<std::string>();
    if (type == "string") {
        v.data = j.at("data").get<std::string>(); 
    } else if (type == "integer") {
        v.data = j.at("data").get<long long>(); 
    }
}

long long getCurrentTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void KeyValueStore::set(const std::string& key, const std::string& value, long long ttl_ms) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
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
    std::lock_guard<std::recursive_mutex> lock(mtx);
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
    std::lock_guard<std::recursive_mutex> lock(mtx);
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
    std::lock_guard<std::recursive_mutex> lock(mtx);
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
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    json final_json = json::object(); // Start with an empty JSON object

    for (const auto& pair : data) {
        if (pair.second.is_expired()) {
            continue; // Don't save expired keys
        }

        // Create a JSON object for the value part
        json value_j = pair.second;
        std::string value_str = value_j.dump();

        // Hash the string representation of the value
        std::string hash_hex_str;
        picosha2::hash256_hex_string(value_str, hash_hex_str);

        // Create the per-entry envelope
        json entry_envelope;
        entry_envelope["value"] = value_j;
        entry_envelope["hash"] = hash_hex_str;

        // Add it to our final JSON object
        final_json[pair.first] = entry_envelope;
    }

    file << final_json.dump(4);
    file.close();
    return true;
}

bool KeyValueStore::load(const std::string& filename) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::ifstream file(filename);
    if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
        return true;
    }

    json file_j;
    try {
        file >> file_j;
    } catch (const json::parse_error& e) {
        std::cerr << "[ERROR] Failed to parse " << filename << ". It is not valid JSON. Starting fresh." << std::endl;
        data.clear();
        return true;
    }

    for (auto& element : file_j.items()) {
        const std::string& key = element.key();
        const json& entry_envelope = element.value();

        if (!entry_envelope.is_object() || !entry_envelope.contains("value") || !entry_envelope.contains("hash")) {
            std::cerr << "[WARNING] Skipping malformed entry for key '" << key << "'. Missing 'value' or 'hash' field." << std::endl;
            continue;
        }
        
        json value_j = entry_envelope["value"];
        std::string stored_hash = entry_envelope["hash"];
        
        // Recalculate hash to verify integrity
        std::string value_str = value_j.dump();
        std::string calculated_hash;
        picosha2::hash256_hex_string(value_str, calculated_hash);
        
        if (stored_hash != calculated_hash) {
            std::cerr << "[CRITICAL] TAMPERING DETECTED for key '" << key << "'. This entry will not be loaded." << std::endl;
            continue; // Skip this entry and move to the next
        }

        // If the hash is valid, deserialize the value
        try {
            data[key] = value_j.get<ValueWithTTL>();
        } catch (const json::exception& e) {
            std::cerr << "[WARNING] Skipping corrupted data for key '" << key << "'. Details: " << e.what() << std::endl;
        }
    }
    
    file.close();
    return true;
}

void KeyValueStore::begin(){
    std::lock_guard<std::recursive_mutex> lock(mtx); 
    if (in_trxn) {
        std::cout << "ERROR: Transaction already in progress." << std::endl;
        return;
    }
    in_trxn = true;
    trxn_data.clear();
    std::cout << "OK" << std::endl;
}

void KeyValueStore::commit() {
    std::lock_guard<std::recursive_mutex> lock(mtx); 
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
    std::lock_guard<std::recursive_mutex> lock(mtx); 
    if (!in_trxn) {
        std::cout << "ERROR: No transaction to rollback." << std::endl;
        return;
    }
    in_trxn = false;
    trxn_data.clear();
    std::cout << "OK" << std::endl;
}

bool KeyValueStore::remove(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (in_trxn) {
        trxn_data[key] = std::nullopt;
        return true;
    }
    get(key);
    return data.erase(key) > 0;
}

size_t KeyValueStore::count() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return data.size();
}