#pragma once
#include "flight_provider.h"
#include "hotel_provider.h"
 
#include <string>
#include <deque>
 
class Trip {
public:
 
    Trip(HotelProvider& hp, FlightProvider& fp) : hotel_provider(hp), flight_provider(fp) {}
 
    Trip() = default;
    Trip(const Trip&) = default;
    Trip(Trip&&) = default;
    Trip& operator=(const Trip&) = default;
    Trip& operator=(Trip&&) = default;
 
    void add_flights(int value) {
        flights.push_back(value);
    }
 
    void add_hotels(int value) {
        hotels.push_back(value);
    }
 
    void Cancel() {
        
        for (size_t i = 0; i < flights.size(); ++i) {
            flight_provider.Cancel(i);
        }
        
        for (size_t i = 0; i < hotels.size(); ++i) {
            hotel_provider.Cancel(i);
        }
    }
    
    ~Trip() {
        Cancel();
    };
    
private:
    HotelProvider& hotel_provider;
    FlightProvider& flight_provider;
    
    std::deque<HotelProvider::BookingId> hotels;
    std::deque<FlightProvider::BookingId> flights;
};
 
class TripManager {
public:
    
    using BookingId = std::string;
    
    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date_from;
        std::string date_to;
    };
 
    Trip Book(const BookingData& data) {
        Trip trip(hotel_provider_, flight_provider_);
        {
            FlightProvider::BookingData flight_booking_data;
            
            flight_booking_data.city_from = data.city_from;
            flight_booking_data.city_to = data.city_to;
            flight_booking_data.date = data.date_from;
            
            trip.add_flights(flight_provider_.Book(flight_booking_data));
        }
        
        {
            HotelProvider::BookingData hotel_booking_data;
            
            hotel_booking_data.city = data.city_to;
            hotel_booking_data.date_to = data.date_to;
            hotel_booking_data.date_from = data.date_from;
            
            trip.add_hotels(hotel_provider_.Book(hotel_booking_data));
        }
        
        {
            FlightProvider::BookingData flight_booking_data;
            
            flight_booking_data.city_from = data.city_from;
            flight_booking_data.city_to = data.city_to;
            flight_booking_data.date = data.date_to;
            
            trip.add_flights(flight_provider_.Book(flight_booking_data));
        }
        
        return trip;
    }
 
    void Cancel(Trip& trip) {
        trip.Cancel();
    }
 
private:
    HotelProvider hotel_provider_;
    FlightProvider flight_provider_;
};