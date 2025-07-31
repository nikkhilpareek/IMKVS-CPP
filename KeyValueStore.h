#ifndef KEYVALUESTORE_H
#define KEYVALUESTORE_H

#include <string>
#include <unordered_map>
#include <optional>
#include <chrono>

#include "json.hpp"
using json = nlohmann::json;

struct ValueWithTTL {
    std::string value;
    long long expiration_time_ms;

    bool is_expired() const {
        if (expiration_time_ms == -1) {
            return false;
        }
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        return now > expiration_time_ms;
    }
};

void to_json(json& j, const ValueWithTTL& v);
void from_json(const json& j, ValueWithTTL& v);

class KeyValueStore {
private:
    std::unordered_map<std::string, ValueWithTTL> data;

    bool in_trxn = false;
    std::unordered_map<std::string, std::optional<ValueWithTTL>> trxn_data;

public:
    void set(const std::string& key, const std::string& value, long long ttl_ms = -1);
    std::optional<std::string> get(const std::string& key);
    bool remove(const std::string& key);
    size_t count() const;
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    void begin();
    void commit();
    void rollback();
};

#endif // KEYVALUESTORE_H