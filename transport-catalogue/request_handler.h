#pragma once
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include <optional>
#include <sstream>


class RequestHandler {
public:
    RequestHandler(const transport_catalogue::TransportCatalogue& catalogue, const renderer::MapRenderer& renderer, const transport_catalogue::Router& router)
        : catalogue_(catalogue)
        , renderer_(renderer)
        , router_(router)
    {
    }

    std::optional<transport_catalogue::BusStat> GetBusStat(const std::string_view bus_number) const;
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;

    bool IsBusNumber(const std::string_view bus_number) const;
    bool IsStopName(const std::string_view stop_name) const;

    const std::optional<graph::RouterInfoRef<double>> GetOptimalRoute(const std::string_view stop_from, const std::string_view stop_to) const;

    svg::Document RenderMap() const;

private:
    const transport_catalogue::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
    const transport_catalogue::Router& router_;
};