#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <iostream>

class JsonReader {
public:
    JsonReader(std::istream& input)
        : input_(json::Load(input))
    {}

    const json::Node& GetBaseRequests() const;
    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;
    const json::Node& GetRoutingSettings() const;

    void ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const;

    void FillCatalogue(transport_catalogue::TransportCatalogue& catalogue);
    renderer::MapRenderer FillRenderSettings(const json::Node& settings) const;
    transport_catalogue::Router FillRoutingSettings(const json::Node& settings) const;

    const json::Node PrintRoute(const json::Dictionary& request_map, RequestHandler& rh) const;
    const json::Node PrintStop(const json::Dictionary& request_map, RequestHandler& rh) const;
    const json::Node PrintMap(const json::Dictionary& request_map, RequestHandler& rh) const;
    const json::Node PrintRouting(const json::Dictionary& request_map, RequestHandler& rh) const;

private:
    json::Document input_;
    json::Node dummy_ = nullptr;

    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> FillStop(const json::Dictionary& request_map) const;
    void FillStopDistances(transport_catalogue::TransportCatalogue& catalogue) const;
    std::tuple<std::string_view, std::vector<const transport_catalogue::Stop*>, bool> FillRoute(const json::Dictionary& request_map, transport_catalogue::TransportCatalogue& catalogue) const;
};