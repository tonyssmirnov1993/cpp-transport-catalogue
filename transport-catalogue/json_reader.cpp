#include "json_reader.h"
#include <sstream>

using namespace std::string_literals;

void JsonReader::LoadJson(json::Document& document, TransportCatalogue& tq, std::vector<StatRequest>& stat_request, map_renderer::Renderer& render)
{
    ParseNode(document.GetRoot(), tq, stat_request, render);
}

void JsonReader::ParseNode(const json::Node& root, TransportCatalogue& tq, std::vector<StatRequest>& stat_request, map_renderer::Renderer& render)
{
    json::Dictionary root_dict;
    if (root.IsMap()) {

        root_dict = root.AsMap();
        ParseRenderRequests(root_dict.at("render_settings"), /*tq,*/ render);
        ParseBaseRequests(root_dict.at("base_requests"), tq);       
        ParseNodeStat(root_dict.at("stat_requests"), stat_request);
        render.AddCoords(tq); 
    }
}

void JsonReader::ParseNodeStat(const json::Node& node, std::vector<StatRequest>& stat_request)
{
    json::Array stat_requests;
    json::Dictionary req_map;
    StatRequest req;

    if(node.IsArray()){
        stat_requests = node.AsArray();
        for (json::Node& req_node : stat_requests) {
            if (req_node.IsMap()) {
                req_map = req_node.AsMap();
                req.id = req_map.at("id").AsInt();
                if (req_map.find("name") != req_map.end()) {
                    req.name = req_map.at("name").AsString();
                }
                
                req.type = req_map.at("type").AsString();
                stat_request.push_back(req);
            }
            else {
                std::cout << "Error: base_requests is not array"s;
            }
        }
    }
}

json::Document JsonReader::ExecuteQueries(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::Renderer& render)
{   //============добавлено 3 функции (разбиение метода)
    std::vector<json::Node> result_queries;

    for (StatRequest req : stat_request) {
        if (req.type == "Stop") {
            result_queries.push_back(UpdExecuteStop(req,catalogue));
        }
        else if (req.type == "Bus") {
            result_queries.emplace_back(UpdExecuteBuses(req, catalogue));
        }
        if (req.type == "Map") {
            result_queries.emplace_back(UpdExecuteMap(req, catalogue, render));
        }       
    }
    return json::Document(json::Node{result_queries});
}

json::Dictionary JsonReader::UpdExecuteStop(StatRequest& req, TransportCatalogue& catalogue)
{
    json::Dictionary result;
    json::Array buses;
    std::vector<std::string> str_buses;
    std::string str_not_f = "not found";

    if (catalogue.GetStop(req.name) == nullptr) {
        result.emplace("request_id", json::Node{ req.id });
        result.emplace("error_message", json::Node{ str_not_f });
       
        return result;
    }
    else {
        Stop* stop = catalogue.GetStop(req.name);
        result.emplace("request_id", json::Node{ req.id });

        for (auto& bus : catalogue.GetUniqueBuses(stop)) {
            str_buses.push_back(bus->name_);

        }
        std::sort(str_buses.begin(), str_buses.end());
        for (std::string& bus_s : str_buses) {
            Bus* bus = catalogue.GetBus(bus_s);
            buses.push_back(bus->name_);//
        }
        result.emplace("buses", json::Node{ buses });
        return result;     
    }   
}

json::Dictionary JsonReader::UpdExecuteBuses(StatRequest& req, TransportCatalogue& catalogue)
{
    json::Dictionary result;
    std::string str_not_f = "not found";
    Bus* bus = catalogue.GetBus(req.name);
   auto stat_bus =  catalogue.GetRouteStatistic(bus);
    if (catalogue.GetBus(req.name) == nullptr) {
        result.emplace("request_id", json::Node{ req.id });
        result.emplace("error_message", json::Node{ str_not_f });
        return result;
    }
    else {
        auto curv = double(stat_bus.stat_distance_ / stat_bus.stat_length_);
        result.emplace("request_id", json::Node{ req.id });
        result.emplace("curvature", json::Node{ curv });
        result.emplace("route_length", json::Node{ stat_bus.stat_distance_ });
        result.emplace("stop_count", json::Node{ int(bus->bus_.size()) });
        result.emplace("unique_stop_count", json::Node{ int(catalogue.GetUniqueStops(bus).size()) });
        return result;
    }
}

json::Dictionary JsonReader::UpdExecuteMap(StatRequest& req, TransportCatalogue& catalogue, map_renderer::Renderer& render)
{
    json::Dictionary result;
    std::ostringstream map_stream;
    std::string line;

    render.PrintMapOut(catalogue, map_stream);

    line = map_stream.str();

    result.emplace("map", json::Node{ line });
    result.emplace("request_id", json::Node{ req.id });
    return result;
}


void JsonReader::ParseBaseRequests(const json::Node& node, TransportCatalogue& tq)
{
    json::Node req_node;
    json::Dictionary req_map;
    json::Array base_req;
   
    if (node.IsArray()) {
        base_req = node.AsArray();
        for (json::Node& node : base_req) {
            if (node.IsMap()) {
                req_map = node.AsMap();
                req_node = req_map.at("type");
                if (req_node.AsString() == "Bus") {

                    ParseBus(node, tq);
                }
                else if (req_node.AsString() == "Stop") {
                    ParseStop(node, tq);
                }
            }           
        }      
        ParseDistance(tq);
    }       
}

void JsonReader::ParseRenderRequests(const json::Node& node,/* TransportCatalogue& tq,*/ map_renderer::Renderer& render)
{
    map_renderer::RenderSettings set;
   
    json::Dictionary rend_req;
    
    if (node.IsMap()) {

        rend_req = node.AsMap();

        set.width = rend_req.at("width").AsDouble();
        set.height = rend_req.at("height").AsDouble();
        set.padding = rend_req.at("padding").AsDouble();
        set.line_width = rend_req.at("line_width").AsDouble();
        set.stop_radius = rend_req.at("stop_radius").AsDouble();
        set.bus_label_font_size = rend_req.at("bus_label_font_size").AsInt();
        set.stop_label_font_size = rend_req.at("stop_label_font_size").AsInt();
        set.underlayer_width = rend_req.at("underlayer_width").AsDouble();
        set.bus_label_offset.x = rend_req.at("bus_label_offset").AsArray()[0].AsDouble();
        set.bus_label_offset.y = rend_req.at("bus_label_offset").AsArray()[1].AsDouble();
        set.stop_label_offset.x = rend_req.at("stop_label_offset").AsArray()[0].AsDouble();
        set.stop_label_offset.y = rend_req.at("stop_label_offset").AsArray()[1].AsDouble();

        ParseUnderColor(node, set);
        ParseColorPalette(node, set);

        render.AddSettings(set);        
    }
}

void JsonReader::ParseUnderColor(const json::Node& node, map_renderer::RenderSettings& settings)
{
    json::Dictionary rend_req = node.AsMap();
    if (rend_req.at("underlayer_color").IsArray()) {
        json::Array under_col = rend_req.at("underlayer_color").AsArray();
        if (under_col.size() == 4) {
            uint8_t a = static_cast<uint8_t>(rend_req.at("underlayer_color").AsArray()[0].AsDouble());
            uint8_t b = static_cast<uint8_t>(rend_req.at("underlayer_color").AsArray()[1].AsDouble());
            uint8_t c = static_cast<uint8_t>(rend_req.at("underlayer_color").AsArray()[2].AsDouble());
            double op = rend_req.at("underlayer_color").AsArray()[3].AsDouble();
            settings.underlayer_color = svg::RGBA{ a,b,c,op };
        }
        else if (under_col.size() == 3) {
            uint8_t a = static_cast<uint8_t>(rend_req.at("underlayer_color").AsArray()[0].AsDouble());
            uint8_t b = static_cast<uint8_t>(rend_req.at("underlayer_color").AsArray()[1].AsDouble());
            uint8_t c = static_cast<uint8_t>(rend_req.at("underlayer_color").AsArray()[2].AsDouble());
            settings.underlayer_color = svg::RGB{ a,b,c };
        }

    }
    else if (rend_req.at("underlayer_color").IsString()) {
        std::string under_color = rend_req.at("underlayer_color").AsString();
        settings.underlayer_color = under_color;
    }

}

void JsonReader::ParseColorPalette(const json::Node& node, map_renderer::RenderSettings& settings)
{
    json::Dictionary rend_req = node.AsMap();
  
    if (rend_req.at("color_palette").IsArray()) {
        for (auto& clr : rend_req.at("color_palette").AsArray()) {
            if (clr.IsString()) {
                settings.colors.emplace_back(clr.AsString());
            }
            else if (clr.IsArray()) {
                if (clr.AsArray().size() == 4) {
                    uint8_t a = static_cast<uint8_t>(clr.AsArray()[0].AsDouble());
                    uint8_t b = static_cast<uint8_t>(clr.AsArray()[1].AsDouble());
                    uint8_t c = static_cast<uint8_t>(clr.AsArray()[2].AsDouble());
                    double op = clr.AsArray()[3].AsDouble();
                    settings.colors.emplace_back(svg::RGBA{ a,b,c,op });
                }
                else if (clr.AsArray().size() == 3) {
                    uint8_t a = static_cast<uint8_t>(clr.AsArray()[0].AsDouble());
                    uint8_t b = static_cast<uint8_t>(clr.AsArray()[1].AsDouble());
                    uint8_t c = static_cast<uint8_t>(clr.AsArray()[2].AsDouble());
                    settings.colors.emplace_back(svg::RGB{ a,b,c });
                }
            }
        }
    }            
}


void JsonReader::ParseBus(const json::Node& node, TransportCatalogue& tq)
{
    std::string type_req = "";
    std::vector<std::string_view> stops;
    std::string name = "";
    bool is_round_trip = false;

    for (auto& [key, value] : node.AsMap()) {
        if (key == "name") {
            name = value.AsString();
        }
        else if (key == "stops") {
            for (auto& stop : value.AsArray()) {
                stops.push_back(stop.AsString());
            }
        }
        else if (key == "is_roundtrip") {
            is_round_trip = value.AsBool();
        }
    }

    if (is_round_trip) {
        tq.AddRoute(name, stops);
        tq.GetBus(name)->is_round_trip_ = true; 
       
    }
    else {
        std::vector<std::string_view> results(stops.begin(), stops.end());
        results.insert(results.end(), std::next(stops.rbegin()), stops.rend());
        tq.AddRoute(name, results);
        tq.GetBus(name)->is_round_trip_ = false; 
    }    
}

void JsonReader::ParseStop(const json::Node& node, TransportCatalogue& tq)
{
    
    std::string stop_name = "";
    geo::Coordinates coordinate_stop{};
    
    stop_name = node.AsMap().at("name").AsString();
    coordinate_stop.lat = node.AsMap().at("latitude").AsDouble();
    coordinate_stop.lng = node.AsMap().at("longitude").AsDouble();

    auto stop = tq.GetStop(stop_name);
    auto& st = node.AsMap().at("road_distances").AsMap();
    
    stops_distances_.insert({ stop_name, st });
    if (stop == nullptr) {
        tq.AddStop(stop_name, coordinate_stop);
        
    }
    if (stop != nullptr && (stop->coordinates_.lat + stop->coordinates_.lng == 0.0)) {
        stop->coordinates_.lat = coordinate_stop.lat;
        stop->coordinates_.lng = coordinate_stop.lng;
    }   
}

void JsonReader::ParseDistance( TransportCatalogue& tq)
{
    std::string prev_station;
    int prev_distance = 0;
  
    for (auto &[station_from, station_to] : stops_distances_) {

        if (station_to.AsMap().empty()) {
            tq.AddDistance(station_from, std::move(prev_station), std::move(prev_distance));
        }
        for (auto& [station_to_name, station_distance] : station_to.AsMap()) {
            prev_station = station_from;
            prev_distance = station_distance.AsInt();

            tq.AddDistance(station_from, station_to_name, std::move(station_distance.AsInt()));
        }         
    }   
}