#include "transport_catalogue.h"

using namespace transport_catalogue::geo;

namespace transport_catalogue {

	Stop* TransportCatalogue::GetStop(std::string_view stop)
	{
		if (stopname_to_stops_.empty()) {
			return nullptr;
		}

		if (stopname_to_stops_.find(stop) != stopname_to_stops_.end()) {
			return stopname_to_stops_.at(stop);
		}
		return nullptr;
	}

	Bus* TransportCatalogue::GetBus(std::string_view bus)
	{
		if (busname_to_bus_.empty()) {
			return nullptr;
		}

		if (busname_to_bus_.find(bus) != busname_to_bus_.end())
		{
			return busname_to_bus_.at(bus);
		}
		return nullptr;
	}


	std::unordered_set <const Bus*>TransportCatalogue::GetUnicBus(Stop* stop)
	{
		std::unordered_set<const Bus*> unq_buses;
		unq_buses.insert(stop->buses_.begin(), stop->buses_.end());
		return unq_buses;
	}

	void TransportCatalogue::AddStop(std::string_view name, const Coordinates coordinate)
	{

		stops_.push_back({ std::string(name), coordinate.lat, coordinate.lng });
		stopname_to_stops_[stops_.back().name_] = &stops_.back();

	}

	void TransportCatalogue::AddRoute(std::string_view bus_name, std::vector<std::string_view>stops)
	{
		Bus bus_tmp;
		bus_tmp.name_ = bus_name;

		for (auto& stop : stops) {

			if (stopname_to_stops_.find(stop) != stopname_to_stops_.end()) {

				bus_tmp.bus_.push_back(stopname_to_stops_.at(stop));
			}
			else {
				stops_.push_back({ std::string(stop), 0.0, 0.0 });
				stopname_to_stops_[stops_.back().name_] = &stops_.back();
				bus_tmp.bus_.push_back(stopname_to_stops_.at(stop));
			}
		}

		buses_.push_back(std::move(bus_tmp)); //deque<Bus>
		busname_to_bus_[buses_.back().name_] = &buses_.back(); //u_map<Bus*>

		//Stop add Bus.name_
		for (auto& stop : stops) {
			stopname_to_stops_.at(stop)->buses_.push_back(&buses_.back());
		}
	}

	std::unordered_set<Stop*> TransportCatalogue::get_uniq_stops(Bus* bus)
	{
		std::unordered_set<Stop*> uniq_stops;
		uniq_stops.insert(bus->bus_.begin(), bus->bus_.end());
		return uniq_stops;
	}
}