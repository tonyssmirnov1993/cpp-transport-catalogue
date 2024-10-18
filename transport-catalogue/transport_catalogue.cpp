#include "transport_catalogue.h"
#include <numeric>
using namespace transport_catalogue::geo;

namespace transport_catalogue {
	const double TransportCatalogue::GetLength(Bus* bus) const
	{
		
		return transform_reduce(next(bus->bus_.begin()),
			bus->bus_.end(),
			bus->bus_.begin(),
			0.0,
			std::plus<>{},
			[](const Stop* lhs, const Stop* rhs) {
				return geo::ComputeDistance({ (*lhs).coordinates_.lat,
										(*lhs).coordinates_.lng }, { (*rhs).coordinates_.lat,
																	 (*rhs).coordinates_.lng });
			});
	}

	const int TransportCatalogue::GetBusDistance(Bus* bus) const
	{
		int result = 0;
		Stop* st1;
		Stop* st2;
		
		for (size_t i = 0; i < bus->bus_.size() - 1; i++)
		{
			st1 = bus->bus_[i];
			st2 = bus->bus_[i + 1];
			auto pair_stop = std::make_pair(st1, st2);
			if (distance_.empty()) {
				return 0;
			}

			if (distance_.find(pair_stop) != distance_.end()) {
				auto f_p_stop = distance_.at(pair_stop);
				result += f_p_stop;
			}
			else {
				pair_stop = std::make_pair(st2, st1);
				if (distance_.find(pair_stop) != distance_.end()) {
					auto f_p_stop = distance_.at(pair_stop);
					result += f_p_stop;
				}				
			}			
		}
		
		return result;
	}

	Statistic TransportCatalogue::GetRouteStatistic(Bus* bus)
	{
		
		Statistic stat;
		stat.stat_distance_ = GetBusDistance(bus);
		stat.stat_length_ = GetLength(bus);
		return stat;
	}

	Stop* TransportCatalogue::GetStop(std::string_view stop)
	{
		if (stopname_to_stops_.empty()) {
			return nullptr;
		}

		std::unordered_map<std::string_view, Stop*>::iterator it_stop = stopname_to_stops_.find(stop);
		if (it_stop != stopname_to_stops_.end()) {
			return it_stop->second;
		}
		return nullptr;
	}

	Bus* TransportCatalogue::GetBus(std::string_view bus)
	{
		if (busname_to_bus_.empty()) {
			return nullptr;
		}
		std::unordered_map<std::string_view, Bus*>::iterator it_bus = busname_to_bus_.find(bus);
		if (it_bus != busname_to_bus_.end())
		{
			return it_bus->second;
		}
		return nullptr;
	}

	const std::unordered_set <const Bus*>&TransportCatalogue::GetUniqueBuses(Stop* stop) const
	{
		if (unique_buses_.empty()) {
			return {};
		}
		std::unordered_map<std::string_view, std::unordered_set<const Bus*>>::const_iterator it_setbus = unique_buses_.find(stop->name_);
		if (it_setbus != unique_buses_.end())
		{
			return it_setbus->second;
		}
		return {};
	}

	void TransportCatalogue::AddStop(std::string_view name, const Coordinates coordinates)
	{
		stops_.push_back({ std::string(name), coordinates.lat, coordinates.lng });
		stopname_to_stops_[stops_.back().name_] = &stops_.back();
	}

	void TransportCatalogue::AddRoute(std::string_view bus_name, const std::vector<std::string_view>&stops)
	{

		Bus bus_temp;
		bus_temp.name_ = bus_name;

		for (auto& stop : stops) {
			if (stopname_to_stops_.find(stop) != stopname_to_stops_.end()) {
				bus_temp.bus_.push_back(stopname_to_stops_.at(stop));
			}
			else {
				stops_.push_back({ std::string(stop), 0.0, 0.0 });
				stopname_to_stops_[stops_.back().name_] = &stops_.back();
				bus_temp.bus_.push_back(stopname_to_stops_.at(stop));
			}
		}

		buses_.push_back(std::move(bus_temp)); //deque<Bus>
		busname_to_bus_[buses_.back().name_] = &buses_.back(); //u_map<Bus*>
		

		for (auto& stop : stops) {
			
			stopname_to_stops_.at(stop)->buses_.push_back(&buses_.back());
			unique_buses_[stopname_to_stops_.at(stop)->name_].insert(&buses_.back());
		}
	}

	std::unordered_set<Stop*> TransportCatalogue::GetUniqueStops(Bus* bus)
	{
		std::unordered_set<Stop*> uniq_stops;
		uniq_stops.insert(bus->bus_.begin(), bus->bus_.end());
		return uniq_stops;
	}

	void TransportCatalogue::AddDistance(std::string_view stop_from, std::string_view stop_to, int distance)
	{
		auto distance_pair = std::make_pair(GetStop(stop_from), GetStop(stop_to));
		distance_.insert({ distance_pair, distance });
	}

	const std::unordered_map<std::string_view, Bus*> &TransportCatalogue::GetBuses() const
	{
		return busname_to_bus_;
	}

	const std::unordered_map<std::string_view, Stop*> &TransportCatalogue::GetStops() const
	{
		return stopname_to_stops_;
	}
}