#include <iostream>
#include <string>
#include <unordered_map>
#include <optional>

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
};

int main() {
    KeyValueStore kvs;

    kvs.set("home", "ganganagar");
    kvs.set("college", "jaipur");
    std::cout << "Set 'home' and 'college'." << std::endl;

    // Test getting a value that exists
    if (auto value = kvs.get("college")) {
        std::cout << "GET 'college': Found value -> " << *value << std::endl;
    }

    // Test removing a value
    if (kvs.remove("college")) {
        std::cout << "REMOVE 'college': Success." << std::endl;
    }

    // Test getting a value that has been removed
    if (auto value = kvs.get("college")) {
        std::cout << "GET 'college': Found value -> " << *value << std::endl;
    } else {
        std::cout << "GET 'college': Value not found." << std::endl;
    }

    return 0;
}