#include "request_handler.h"
 
namespace request_handler {
 
RequestHandler::RequestHandler(TransportCatalogue& db, const MapRenderer& renderer) : db_(db), 
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
    
std::vector<std::string_view> RequestHandler::get_sort_buses_names() const {
    std::vector<std::string_view> buses_names;
    
    auto buses = db_.get_busname_to_bus();
    if (buses.size() > 0) {
        
        for (auto& [busname, bus] : buses) {
            buses_names.push_back(busname);
        }   
 
        std::sort(buses_names.begin(), buses_names.end());
        
        return buses_names;
    } else {
        return {};
    }
}
 
void RequestHandler::render_map_add_line(std::vector<std::pair<Bus*, int>>& buses_palette, 
                                         svg::Document& doc,
                                         SphereProjector& sphere_projector) const {
    std::vector<geo::Coordinates> stops_geo_coords;
       
    for (auto [bus, palette] : buses_palette) {  
        for (auto& stop : bus->stops) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop->latitude;
            coordinates.longitude = stop->longitude;
            
            stops_geo_coords.push_back(coordinates); 
        }
        svg::Polyline bus_line;
        bool bus_empty = true;   
        
        for (auto& coord : stops_geo_coords) {
            bus_empty = false;
            bus_line.add_point(sphere_projector(coord));
        } 
        
        if (!bus_empty) {
            map_renderer_.set_line_properties(bus_line, palette);
            doc.add(bus_line);   
        }
        stops_geo_coords.clear();
    }
}
    
void RequestHandler::render_map_add_buses_name(std::vector<std::pair<Bus*, int>>& buses_palette, 
                                               svg::Document& doc,
                                               SphereProjector& sphere_projector) const {
    std::vector<geo::Coordinates> stops_geo_coords;
    bool bus_empty = true; 
    
    for (auto [bus, palette] : buses_palette) {  
        
        for (auto& stop : bus->stops) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop->latitude;
            coordinates.longitude = stop->longitude;
            
            stops_geo_coords.push_back(coordinates); 
            
            if(bus_empty) bus_empty = false;
        }
        
        svg::Text route_name_roundtrip;
        svg::Text route_title_roundtrip;
        svg::Text route_name_notroundtrip;
        svg::Text route_title_notroundtrip; 
        
        if (!bus_empty) {
            if (bus->is_roundtrip) {
                map_renderer_.set_route_text_additional_properties(route_name_roundtrip,
                                                                   std::string(bus->name),
                                                                   sphere_projector(stops_geo_coords[0]));
                doc.add(route_name_roundtrip);
                
                map_renderer_.set_route_text_color_properties(route_title_roundtrip,
                                                              std::string(bus->name),
                                                              palette,
                                                              sphere_projector(stops_geo_coords[0]));
                doc.add(route_title_roundtrip);
                
            } else {
                
                map_renderer_.set_route_text_additional_properties(route_name_roundtrip, 
                                                                   std::string(bus->name),
                                                                   sphere_projector(stops_geo_coords[0]));
                doc.add(route_name_roundtrip);
                
                map_renderer_.set_route_text_color_properties(route_title_roundtrip,
                                                              std::string(bus->name),
                                                              palette,
                                                              sphere_projector(stops_geo_coords[0]));
                doc.add(route_title_roundtrip);
                
                if (stops_geo_coords[0] != stops_geo_coords[stops_geo_coords.size()/2]) {
                    
                    map_renderer_.set_route_text_additional_properties(route_name_notroundtrip,
                                                                       std::string(bus->name),
                                                                       sphere_projector(stops_geo_coords[stops_geo_coords.size()/2]));
                    doc.add(route_name_notroundtrip);
                    
                    map_renderer_.set_route_text_color_properties(route_title_notroundtrip,
                                                                  std::string(bus->name),
                                                                  palette,
                                                                  sphere_projector(stops_geo_coords[stops_geo_coords.size()/2]));
                    doc.add(route_title_notroundtrip);
                }
            } 
        }
        
        bus_empty = false;
        stops_geo_coords.clear();
    }
}
    
void RequestHandler::render_map_add_stops_circle(svg::Document& doc,
                                                 SphereProjector& sphere_projector) const {  
    std::vector<geo::Coordinates> stops_geo_coords;    
    Stop* stop_info;
    auto stops = db_.get_stopname_to_stop();    
    std::vector<std::string_view> stops_name;
 
    for (auto& [stop_name, stop] : stops) {
        
        if (stop->buses.size() > 0) {
            stops_name.push_back(stop_name);
        }
    }
 
    std::sort(stops_name.begin(), stops_name.end());
    
    svg::Circle icon;
    
    for(std::string_view stop_name : stops_name){
        stop_info = db_.get_stop(stop_name);
        
        if (stop_info) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop_info->latitude;
            coordinates.longitude = stop_info->longitude;            
            
            map_renderer_.set_stops_circles_properties(icon,
                                                       sphere_projector(coordinates));
            doc.add(icon);
        }
    }
}
  
void RequestHandler::render_map_add_stops_name(svg::Document& doc, 
                                               SphereProjector& sphere_projector) const {
    std::vector<geo::Coordinates> stops_geo_coords;    
    Stop* stop_info;
    auto stops = db_.get_stopname_to_stop();    
    std::vector<std::string_view> stops_name;
 
    for (auto& [stop_name, stop] : stops) {
        
        if (stop->buses.size() > 0) {
            stops_name.push_back(stop_name);
        }
    }
 
    std::sort(stops_name.begin(), stops_name.end());
    
    svg::Text svg_stop_name;
    svg::Text svg_stop_name_title;
    
    for (std::string_view stop_name : stops_name) {
        stop_info = db_.get_stop(stop_name);
        
        if (stop_info) {
            geo::Coordinates coordinates;
            coordinates.latitude = stop_info->latitude;
            coordinates.longitude = stop_info->longitude;            
            
            map_renderer_.set_stops_text_additional_properties(svg_stop_name, 
                                                               stop_info->name, 
                                                               sphere_projector(coordinates));
            doc.add(svg_stop_name);
            
            map_renderer_.set_stops_text_color_properties(svg_stop_name_title, 
                                                          stop_info->name, 
                                                          sphere_projector(coordinates));
            doc.add(svg_stop_name_title); 
        }
    }
}
    
void RequestHandler::render_map(std::ostream& out) const {
    SphereProjector sphere_projector = map_renderer_.get_sphere_projector(get_stops_coordinates());
    svg::Document doc;
    std::vector<std::pair<Bus*, int>> buses_palette;
 
    int palette_size = 0;
    int palette_index = 0;
    
    palette_size = map_renderer_.get_palette_size();
    if (palette_size == 0) {
        std::cout << "color palette is empty";
        return;
    }
    
    auto buses = db_.get_busname_to_bus();   
    if (buses.size() > 0) {
        
        for (std::string_view bus_name : get_sort_buses_names()) {
            Bus* bus_info = db_.get_bus(bus_name);
 
            if (bus_info) {  
                if (bus_info->stops.size() > 0) {
                    buses_palette.push_back(std::make_pair(bus_info, 
                                                           palette_index));
                    palette_index++;
                    
                    if (palette_index == palette_size) {
                        palette_index = 0;
                    }
                }
            }
        }
 
        if (buses_palette.size() > 0) {
            render_map_add_line(buses_palette, 
                                doc, 
                                sphere_projector);
            
            render_map_add_buses_name(buses_palette, 
                                      doc, 
                                      sphere_projector);            
        }          
    }
    
    render_map_add_stops_circle(doc, sphere_projector);
    render_map_add_stops_name(doc, sphere_projector);
    doc.render(out);
}
    
}//end namespace request_handler