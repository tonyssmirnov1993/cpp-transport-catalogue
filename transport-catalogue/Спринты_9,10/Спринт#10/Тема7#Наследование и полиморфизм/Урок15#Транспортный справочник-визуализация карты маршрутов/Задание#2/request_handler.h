#pragma once
 
#include "transport_catalogue.h"
#include "map_renderer.h"
 
using namespace transport_catalogue;
using namespace map_renderer;
 
namespace request_handler {
 
using BusPoints = std::vector<std::pair<std::string_view, std::vector<geo::Coordinates>>>;
 
class RequestHandler {
public:
    
    RequestHandler(TransportCatalogue& db, const MapRenderer& map_renderer_);
    
    std::vector<geo::Coordinates> get_stops_coordinates() const;
    std::vector<std::string_view> get_sort_buses_names() const;
    
    void render_map_add_line(std::vector<std::pair<Bus*, int>>& buses_palette, svg::Document& doc, SphereProjector& sphere_projector) const;
    void render_map_add_buses_name(std::vector<std::pair<Bus*, int>>& buses_palette, svg::Document& doc, SphereProjector& sphere_projector) const;
    void render_map_add_stops_circle(svg::Document& doc, SphereProjector& sphere_projector) const;
    void render_map_add_stops_name(svg::Document& doc, SphereProjector& sphere_projector) const;
    void render_map(std::ostream& out) const;
 
private:
    TransportCatalogue& db_;
    const MapRenderer& map_renderer_;
};
    
}//end namespace request_handler