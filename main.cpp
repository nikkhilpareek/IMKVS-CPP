#include <string>
#include <iostream>
#include <unordered_map>
#include <string>
#include <optional>

class KeyValueStore {
private:
    // What declaration should go here to store our key-value pairs?
    std::unordered_map<std::string, std::string> data;

public:
    // We'll add our functions (methods) here later.
    void set(const std::string &key, const std::string &value){
        data[key] = value;
        std::cout<<"OK\n";
    }

    std::optional<std::string> get(const std::string &key) const {
        auto it = data.find(key);
        if(it == data.end()){
            return std::nullopt;
        }else{
            return it->second;
        }
    }

};

int main(){
    KeyValueStore kvs;
    kvs.set("home","ganganagar");
    kvs.set("college","jaipur");
    
    if(auto value = kvs.get("school")){
        std::cout << "Found value: " << *value << std::endl;
    }else{
        std::cout<<"Value not Found"<<std::endl;
    }

    if(auto value = kvs.get("college")){
        std::cout << "Found value: " << *value << std::endl;
    }else{
        std::cout<<"Value not Found"<<std::endl;
    }
    

}