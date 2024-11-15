#include "json_reader.h"
#include "request_handler.h"
#include <fstream>
#include <iostream>

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    std::ifstream input("primer1.json");
    JsonReader json_doc(input);
    json_doc.FillCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);
    const auto& routing_settings = json_doc.FillRoutingSettings(json_doc.GetRoutingSettings());
    const transport_catalogue::Router router = { routing_settings, catalogue };

    RequestHandler rh(catalogue, renderer, router);
    json_doc.ProcessRequests(stat_requests, rh);

}