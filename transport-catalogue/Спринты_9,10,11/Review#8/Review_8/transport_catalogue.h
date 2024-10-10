#pragma once
#include "geo.h"

#include <deque>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace transport {

    struct BusInfo {
        std::string name;
        std::vector<std::string> stops;
    };

    struct StopInfo {
        std::string name;
        geo::Coordinates coordinates;
    };

    struct StatRoute {
        size_t number_of_stops;
        size_t unique_stops;
        double route_length = 0;
    };

    class TransportCatalogue {
    public:
        void AddBus(const std::string& id, const std::vector<std::string>& stops);
        void AddStop(const std::string& id, const geo::Coordinates& coordinates);
        const BusInfo* GetBus(const std::string& id) const;
        const StopInfo* GetStop(const std::string& id) const;
        StatRoute GetStatBus(const std::string& id) const;
        std::set<std::string_view> GetStatStop(const std::string& id) const;
    private:
        std::deque<BusInfo> buses_;
        std::deque<StopInfo> stops_;
        std::unordered_map<std::string_view, const BusInfo*> buses_info_;
        std::unordered_map<std::string_view, const StopInfo*> stops_info_;
        std::unordered_map<std::string_view, std::set<std::string_view>> buses_on_stops_;
    };
}//namespace transport    