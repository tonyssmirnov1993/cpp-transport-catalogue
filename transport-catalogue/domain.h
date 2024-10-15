#pragma once
#include <string>
#include <vector>
#include "geo.h"

namespace transport_catalogue {

	struct Bus;

	struct Stop {


		std::string name_;
		geo::Coordinates coordinates_;
		std::vector<Bus*>buses_{};
	};

	struct Bus {

		std::string name_;
		std::vector<Stop*>bus_;
		bool is_round_trip_; 
	};
}