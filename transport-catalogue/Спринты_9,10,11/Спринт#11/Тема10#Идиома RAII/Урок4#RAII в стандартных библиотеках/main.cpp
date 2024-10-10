#include <iostream>
#include <deque>
#include <map>
#include <algorithm>
 
using namespace std;
 
struct Client {
    int client_id = 0;
    int room_count = 0;
    int time = 0;
};
 
class HotelManager {
public:
    
    static const int SEC_IN_DAY = 86400;
    
    void Book(int client_id, int room_count, const string& hotel_name, int time) {
        current_time_ = time;
        const Client book{client_id, 
                          room_count, 
                          time};
        
        auto duplicate_id = [&]() {
            std::deque<Client>& hotel = all_bookings_[hotel_name];
            
            auto it = std::find_if(hotel.begin(),
                                   hotel.end(),
                                   [client_id](const Client& client) {
                                      return client.client_id == client_id;
                                   });
            
            
            if (it == hotel.end()) {
                all_bookings_[hotel_name].push_back(std::move(book));
            } else {
                it->room_count += room_count;
                it->time = time;
            }
        };
        duplicate_id();
    }
 
    int ComputeClientCount(string& hotel_name) {
        int yesterday = current_time_ - SEC_IN_DAY;
        int result = 0;
        
        if (!all_bookings_.count(hotel_name)) { 
            return result;
        }
 
        for (auto& [id, _, time] : all_bookings_.at(hotel_name)) {
            
            if (yesterday < time && time <= current_time_) {
                ++result;
            }
        }
        
        return result;
    }
 
    int ComputeRoomCount(string& hotel_name) {
        int yesterday = current_time_ - SEC_IN_DAY;
        int result = 0;
 
        if (!all_bookings_.count(hotel_name)) {
            return result; 
        }
 
        for (auto& [_, room_count, time] : all_bookings_.at(hotel_name)) {
            
            if (yesterday < time && time <= current_time_) {
                result += room_count;
            }
        }
        
        return result;
    }
 
private:
    int current_time_ = 0;
    map<string, deque<Client>> all_bookings_;
};
 
int main() {
    HotelManager manager;
 
    int query_count;
    cin >> query_count;
 
    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
 
        if (query_type == "BOOK") {
            int64_t time;
            cin >> time;
            string hotel_name;
            cin >> hotel_name;
            int client_id, room_count;
            cin >> client_id >> room_count;
            manager.Book(client_id, room_count, hotel_name, time);
        }
        else {
            string hotel_name;
            cin >> hotel_name;
            if (query_type == "CLIENTS") {
                cout << manager.ComputeClientCount(hotel_name) << "\n";
            }
            else if (query_type == "ROOMS") {
                cout << manager.ComputeRoomCount(hotel_name) << "\n";
            }
        }
    }
 
    return 0;
}