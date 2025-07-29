#ifndef KEYVALUESTORE_H
#define KEYVALUESTORE_H

#include <string>
#include <unordered_map>
#include <optional>

class KeyValueStore {
private:
    std::unordered_map<std::string, std::string> data;

public:
    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    bool remove(const std::string& key);
    size_t count() const;
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
};

#endif // KEYVALUESTORE_H