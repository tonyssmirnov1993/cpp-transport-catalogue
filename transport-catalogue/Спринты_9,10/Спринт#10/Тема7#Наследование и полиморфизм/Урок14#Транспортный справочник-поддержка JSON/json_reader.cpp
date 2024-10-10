#include "json_reader.h"
 
namespace transport_catalogue {
namespace detail {
namespace json {
 
void Parse(Document& doc, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request) {
    ParseNode(doc.GetRoot(), 
              catalogue, 
              stat_request);
}
    
void ParseNode(const Node& root, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request) {   
    Dict root_dict;
    
    if (root.IsMap()) {
        root_dict = root.AsMap();
 
        try {          
            ParseNodeBase(root_dict.at("base_requests"), catalogue);
        } catch(...) {
            std::cout << "Error: base_requests is empty";
        }
 
        try {
            ParceNodeStat(root_dict.at("stat_requests"), stat_request);
        } catch(...) {
            std::cout << "Error: stat_requests is empty";
        }
        
    } else {
        std::cout << "Error: root is not map";
    }
}
 
void ParseNodeBase(const Node& root, TransportCatalogue& catalogue) {
    Array base_requests;
    Dict req_map;
    Node req_node;
    
    std::vector<Node> buses;
    std::vector<Node> stops;
    
    if (root.IsArray()) {
        base_requests = root.AsArray();
        
        for (Node& node : base_requests) {            
            if (node.IsMap()) {
                req_map = node.AsMap();
                
                try {
                    req_node = req_map.at("type");
                    if (req_node.IsString()) {
                        
                        if (req_node.AsString() == "Bus") {
                            buses.push_back(req_map);
                            
                        } else if (req_node.AsString() == "Stop") {
                            stops.push_back(req_map);
                            
                        } else {
                            std::cout << "Error: base_requests have bad type";
                        }
                    }
                    
                } catch(...) {
                    std::cout << "Error: base_requests not have type value";
                }                
            }
        }   
        
        for (auto stop : stops) {
            catalogue.add_stop(ParseNodeStop(stop));
        }
        
        for (auto stop : stops) {
            catalogue.add_distance(ParseNodeDistances(stop, catalogue));
        }
        
        for (auto bus : buses) {
            catalogue.add_bus(ParseNodeBus(bus, catalogue));
        }
        
    } else {
        std::cout << "Error: base_requests is not array";
    }
}
 
Stop ParseNodeStop(Node& node) {
    Stop stop;
    Dict stop_node;
    
    if (node.IsMap()) {
        
        stop_node = node.AsMap();
        stop.name = stop_node.at("name").AsString();
        stop.latitude = stop_node.at("latitude").AsDouble();
        stop.longitude = stop_node.at("longitude").AsDouble();
    }
    
    return stop;
}
    
std::vector<Distance> ParseNodeDistances(Node& node, TransportCatalogue& catalogue) {
    std::vector<Distance> distances;
    Dict stop_node;
    Dict stop_road_map;
    std::string begin_name;
    std::string last_name;
    int distance;
    
    if (node.IsMap()) {
        stop_node = node.AsMap();
        begin_name = stop_node.at("name").AsString();
        
        try {
            stop_road_map = stop_node.at("road_distances").AsMap();
            
            for (auto [key, value] : stop_road_map) {
                last_name = key;
                distance = value.AsInt();
                distances.push_back({catalogue.get_stop(begin_name), 
                                     catalogue.get_stop(last_name), 
                                     distance});
            }            
        }
        
        catch(...) {
            std::cout << "Error: Road invalide" << std::endl;
        }        
    }
    
    return distances;
}
 
Bus ParseNodeBus(Node& node, [[maybe_unused]] TransportCatalogue& catalogue) {
    Bus bus;
    Dict bus_node;
    Array bus_stops;
    
    if (node.IsMap()) {
        bus_node = node.AsMap();
        bus.name = bus_node.at("name").AsString();
        bus.is_roundtrip = bus_node.at("is_roundtrip").AsBool();
        
        try {
            bus_stops = bus_node.at("stops").AsArray();
            
            for (Node stop : bus_stops) {
                bus.stops.push_back(catalogue.get_stop(stop.AsString()));
            }
 
            if (!bus.is_roundtrip) {
                size_t size = bus.stops.size() - 1;
                
                for (size_t i = size; i > 0; i--) {
                    bus.stops.push_back(bus.stops[i-1]);
                }
            }   
            
        } catch(...) {
            std::cout << "Error: base_requests: bus: stops is empty" << std::endl;
        }        
    }
    
    return bus;
}
 
void ParceNodeStat(const Node& node, std::vector<StatRequest>& stat_request) {
    Array stat_requests;
    Dict req_map;
    StatRequest req;
    
    if (node.IsArray()) {
        stat_requests = node.AsArray();
        
        for (Node req_node : stat_requests) {
            
            if (req_node.IsMap()) {
                
                req_map = req_node.AsMap();
                req.id = req_map.at("id").AsInt();
                req.type = req_map.at("type").AsString();
                req.name = req_map.at("name").AsString();
                stat_request.push_back(req);
            }
        }
        
    } else {
        std::cout << "Error: base_requests is not array";
    }
}
 
Document ExecuteQueries([[maybe_unused]] TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests) {
    std::vector<Node> result_request;
    
    for (StatRequest req : stat_requests) {
        
        if (req.type == "Stop") {
            result_request.push_back(ExecuteMakeNodeStop(req.id, stop::query(catalogue,
                                                                             req.name)));
 
        } else if(req.type == "Bus") {
            result_request.push_back(ExecuteMakeNodeBus(req.id,bus::query(catalogue,
                                                                          req.name)));
        }        
    }
    return Document{Node{result_request}};
}
 
Node ExecuteMakeNodeStop(int id_request, StopQueryResult stop_info) {
    Dict result;
    Array buses;
    std::string str_not_found = "not found";
    
    if (stop_info.not_found) {
        result.emplace("request_id", Node{id_request});
        result.emplace("error_message", Node{str_not_found});
        
    } else {
        result.emplace("request_id", Node{id_request});
        
        for (std::string bus_name : stop_info.buses_name) {
            buses.push_back(Node{bus_name});
        }
        
        result.emplace("buses", Node{buses});
    }
    
    return Node{result};
}
 
Node ExecuteMakeNodeBus(int id_request, BusQueryResult bus_info) {
    Dict result;
    std::string str_not_found = "not found";
    
    if (bus_info.not_found) {
        result.emplace("request_id", Node{id_request});
        result.emplace("error_message", Node{str_not_found});
        
    } else {
        result.emplace("request_id", Node{id_request});        
        result.emplace("curvature", Node{bus_info.curvature});
        result.emplace("route_length", Node{bus_info.route_length});
        result.emplace("stop_count", Node{bus_info.stops_on_route});
        result.emplace("unique_stop_count", Node{bus_info.unique_stops});
        
    }
    
    return Node{result};
}
    
}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue