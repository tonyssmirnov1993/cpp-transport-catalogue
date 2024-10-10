#pragma once
 
#include <utility>
#include <cassert>
 
namespace raii {
 
template <typename Provider>
class Booking {
public:
    using BookingId = typename Provider::BookingId;
        
    Booking(Provider* p, BookingId id) : provider_(p), id_(id) {}
    
    Booking(const Booking&) = delete;        
    Booking& operator=(const Booking&) = delete;
    
    Booking(Booking&& other) { 
    
        assert(provider_ == nullptr);
        provider_ = std::exchange(other.provider_, nullptr);
        id_ = std::move(other.id_);
        
    }
    
    Booking& operator=(Booking&& other) {
    
        if (provider_ != other.provider_) {
            provider_ = std::exchange(other.provider_, nullptr);
            id_ = std::move(other.id_);
        }
        
        return *this;
    }
    
    ~Booking() {
    
        provider_->CancelOrComplete(*this);
    }
 
    BookingId GetId() const {
    
        return id_;
    } 
 
private:
    Provider* provider_ = nullptr;
    BookingId id_;
    
};
    
}//end namespace booking