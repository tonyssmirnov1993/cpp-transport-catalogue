#pragma once
#include <algorithm>
#include <string_view>
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include "geo.h"

using namespace transport_catalogue::geo;

namespace transport_catalogue {

	struct Bus;
	struct Stop {

		std::string name_;
		double latitude_ = 0.0;
		double longitude_ = 0.0;
		std::vector<Bus*>buses_{};
	};

	struct Bus {

		std::string name_;
		std::vector<Stop*>bus_;
	};

	struct PairHasher {
		size_t operator()(const std::pair<const Stop*, const Stop*>& key)const {
			size_t first_hash = std::hash<const void*>{}(key.first);
			size_t first_second = std::hash<const void*>{}(key.second);
			return 39 * first_hash + first_second;
		}
	};

	class TransportCatalogue {
	public:
		int GetBusDistance(Bus* bus);
		Stop* GetStop(std::string_view stop);
		Bus* GetBus(std::string_view bus);
		std::unordered_set <const Bus*> GetUniqueBuses(Stop* stop);
		void AddStop(std::string_view name, const Coordinates coordinate);
		void AddRoute(std::string_view bus_name, const std::vector<std::string_view> &stops);
		std::unordered_set<Stop*> GetUniqueStops(Bus* bus);
		void AddDistance(std::string_view stop_from, std::string_view stop_to, int dist);

	private:
		std::deque<Stop> stops_;
		std::deque<Bus> buses_;
		std::unordered_map<std::string_view, Stop*> stopname_to_stops_;
		std::unordered_map<std::string_view, Bus*> busname_to_bus_;
		std::unordered_map<std::pair<const Stop*, const Stop*>, int, PairHasher> distance_;
	};
}