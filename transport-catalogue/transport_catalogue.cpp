#include "transport_catalogue.h"


namespace transport_catalogue {

    void TransportCatalogue::AddBus(const std::string& id, const std::vector<std::string>& stops) {
        buses_.push_back({ id, stops });
        for (const auto& stop : stops) {
            buses_for_stop_[stop].insert(id);
        }
        busname_to_bus_[buses_.back().id] = &buses_.back(); //*
    }

    void TransportCatalogue::AddStop(const std::string& id, geo::Coordinates& coordinates) {
        stops_.push_back({ id, coordinates });
        stopname_to_stops_[stops_.back().id] = &stops_.back();
    }

    void TransportCatalogue::AddDistances(const std::string& start, const std::string& finish, unsigned int distance) {
        auto start_stop = FindStop(start);
        auto reach_stop = FindStop(finish);
        distances_.insert({ {start_stop, reach_stop}, distance });
    }

    const Bus* TransportCatalogue::FindBus(const std::string& id) const {
        if (busname_to_bus_.empty()) {
            return nullptr;
        }

        if (busname_to_bus_.find(id) != busname_to_bus_.end())
        {
            return busname_to_bus_.at(id);
        }
        return nullptr;
    }

    const Stop* TransportCatalogue::FindStop(const std::string& id) const {
        if (stopname_to_stops_.empty()) {
            return nullptr;
        }

        if (stopname_to_stops_.find(id) != stopname_to_stops_.end()) {
            return stopname_to_stops_.at(id);
        }
        return nullptr;
    }

    unsigned int TransportCatalogue::GetDistance(const Stop* start_stop, const Stop* reach_stop) const {
        if (distances_.count({ start_stop, reach_stop })) {
            return distances_.at({ start_stop, reach_stop });
        }
        else if (distances_.count({ reach_stop, start_stop })) {
            return distances_.at({ reach_stop, start_stop });
        }
        return 0;
    }

    BusInfo TransportCatalogue::GetBusInfo(const Bus* bus) const {

        BusInfo info;
        info.stops = bus->stops.size();

        std::unordered_set<std::string_view> unique_stops;
        for (const auto& stop : bus->stops) {
            unique_stops.insert(stop);
        }
        info.unique_stops = unique_stops.size();

        unsigned int route_length = 0;
        double geo_length = 0.0;
        for (size_t i = 0; i < bus->stops.size() - 1; ++i) {

            auto first_stop = stopname_to_stops_.find(bus->stops[i]);
            auto second_stop = stopname_to_stops_.find(bus->stops[i + 1]);
            route_length += GetDistance(FindStop(first_stop->second->id), FindStop(second_stop->second->id));
            geo_length += geo::ComputeDistance(first_stop->second->coordinates, second_stop->second->coordinates);


        }

        info.route_length = route_length;
        info.curvature = (double)route_length / geo_length;

        return info;
    }

    const std::set<std::string> *TransportCatalogue::GetStopInfo(const Stop* stop) const {
        if (buses_for_stop_.count(stop->id)) {
            return &(buses_for_stop_.at(stop->id));
        }
        return {};
    }
} //transport_catalogue