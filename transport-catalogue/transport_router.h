#pragma once
#include "router.h"
#include "transport_catalogue.h"
#include <memory>

namespace transport_catalogue {
	static const double KMH = 100.0 / 6.0;
	class Router {
	public:
		Router() = default;		

		Router(const RoutingSettings& settings, const TransportCatalogue& catalogue) {
			settings_.bus_wait_time = settings.bus_wait_time;
			settings_.bus_velocity = settings.bus_velocity;
			BuildGraph(catalogue);
		}
		
	const std::optional<graph::RouterInfoRef<double>> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

	private:
		const graph::DirectedWeightedGraph<double>& GetGraph() const;	
		const graph::DirectedWeightedGraph<double>& BuildGraph(const TransportCatalogue& catalogue);
		graph::DirectedWeightedGraph<double> graph_;
		std::map<std::string, graph::VertexId> stop_ids_;
		std::unique_ptr<graph::Router<double>> router_;
		RoutingSettings settings_;
	};
}