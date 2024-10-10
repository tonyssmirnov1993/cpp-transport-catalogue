#include "request_handler.h"
 
namespace request_handler {
 
RequestHandler::RequestHandler(TransportCatalogue& db,const MapRenderer& renderer) : db_(db), 
                                                                                     map_renderer_(renderer) {}
 
std::vector<geo::Coordinates> RequestHandler::get_stops_coordinates() const {
    
    std::vector<geo::Coordinates> stops_coordinates; 
    auto buses = db_.get_busname_to_bus();
    
    for (auto& [busname, bus] : buses) {
        for (auto& stop : bus->stops) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop->latitude;
            coordinates.longitude = stop->longitude;
            
            stops_coordinates.push_back(coordinates);
        }
    }
    return stops_coordinates;
}
 
void RequestHandler::render_map(std::ostream& out) const {
    svg::Document document;
    [[maybe_unused]] auto sphere_projector = map_renderer_.get_sphere_projector(get_stops_coordinates());
    std::vector<std::string_view> buses_name; 
    std::vector<std::pair<Bus*, int>> buses_palette;
    std::vector<geo::Coordinates> stops_geo_coords;    
    
    int palette_size = 0;
    int palette_index = 0;
    
    palette_size = map_renderer_.get_palette_size();
    if (palette_size == 0) {
        std::cout << "color palette is empty";
        return;
    }
    
    auto buses = db_.get_busname_to_bus();
    if (buses.size() > 0) {
        
        for (auto& [busname, bus] : buses) {
            buses_name.push_back(busname);
        }   
 
        std::sort(buses_name.begin(), buses_name.end());
 
        for (std::string_view bus_name : buses_name) {
            Bus* bus_info = db_.get_bus(bus_name);
 
            if (bus_info) {  
 
                if (bus_info->stops.size() > 0) {
                    buses_palette.push_back(std::make_pair(bus_info, palette_index));
                    palette_index++;
                    
                    if (palette_index == palette_size) {
                        palette_index = 0;
                    }
                }
            }
        }
 
        if (buses_palette.size() > 0) {
            
            for (auto [bus, palette] : buses_palette) {
                
                for (auto& stop : bus->stops) {
                    geo::Coordinates coordinates;
                    coordinates.latitude = stop->latitude;
                    coordinates.longitude = stop->longitude; 
                    stops_geo_coords.push_back(coordinates);
                }
                               
                svg::Polyline bus_line;
                bool bus_empty = true;                
                
                for (auto& point : stops_geo_coords) {
                    bus_empty = false;
                    bus_line.add_point(sphere_projector(point));
                } 
                
                if (!bus_empty) {
                    map_renderer_.set_line_properties(bus_line, palette);
                    document.add(bus_line);
                }
                
                stops_geo_coords.clear();
            }            
        }            
    }  
    
    document.render(out);
}
    
}//end namespace request_handler