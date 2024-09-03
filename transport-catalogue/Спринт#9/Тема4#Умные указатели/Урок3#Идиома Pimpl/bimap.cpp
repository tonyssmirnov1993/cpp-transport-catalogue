#include "bimap.h"
#include <unordered_map>
#include <deque>
#include <stdexcept>
 
// Определение структуры KeyVal
struct KeyVal
{
    std::string key;
    std::string value;
};
 
// Определение структуры BiMap::Impl
struct BiMap::Impl {
    std::unordered_map<std::string_view, std::string_view> key_to_value;
    std::unordered_map<std::string_view, std::string_view> value_to_key;
    std::deque<KeyVal> storage;
 
    Impl() = default;
 
    Impl(const Impl& other) {
        
        key_to_value = other.key_to_value;
        value_to_key = other.value_to_key;
        storage = other.storage;
    }
 
    Impl& operator=(const Impl& other) {
        if (this != &other) {
          
            key_to_value = other.key_to_value;
            value_to_key = other.value_to_key;
            storage = other.storage; 
        }
        return *this;
    }
    
  
 
   bool Add(std::string_view key, std::string_view value) {
        try {
 
            if (key_to_value.count(key) > 0 || value_to_key.count(value) > 0) {
                return false;
            }
 
            storage.push_back({ std::string(key), std::string(value) });
            auto &[storage_key, storage_value] = storage.back();
            key_to_value[storage_key] = storage_value;
            value_to_key[storage_value] = storage_key;
            
            return true;
        }
        catch (const std::bad_alloc&) {
            // частично внесенные изменения
            key_to_value.clear();
            value_to_key.clear();
            storage.clear();
            throw;
        }
    }
                                                             
    
    std::optional<std::string_view> FindValue(std::string_view key) const noexcept {
        auto it = key_to_value.find(key);
        if (it != key_to_value.end()) {
            return it->second;
        }
        return std::nullopt;
    }
 
    std::optional<std::string_view> FindKey(std::string_view value) const noexcept {
        auto it = value_to_key.find(value);
        if (it != value_to_key.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};
 
// Реализация методов BiMap
 
BiMap::BiMap() : impl_(std::make_unique<Impl>()) {}
 
BiMap::BiMap(const BiMap& other) : impl_(std::make_unique<Impl>(*other.impl_)) {}
 
BiMap& BiMap::operator=(const BiMap& other) {
    if (this != &other) {
        *impl_ = *other.impl_; 
    }
    return *this;
}
 
BiMap::BiMap(BiMap&& other) noexcept = default;
 
BiMap& BiMap::operator=(BiMap&& other) noexcept = default;
 
BiMap::~BiMap() = default;
 
bool BiMap::Add(std::string_view key, std::string_view value) {
    return impl_->Add(key, value);
}
 
std::optional<std::string_view> BiMap::FindValue(std::string_view key) const noexcept {
    return impl_->FindValue(key);
}
 
std::optional<std::string_view> BiMap::FindKey(std::string_view value) const noexcept {
    return impl_->FindKey(value);
}