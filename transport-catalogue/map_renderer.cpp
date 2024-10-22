#include "map_renderer.h"
#include "transport_catalogue.h"
namespace map_renderer {

    std::vector<std::string> Renderer::GetSortedBuses(transport_catalogue::TransportCatalogue& tq)
    {
        std::vector<std::string> bus_names;
      
        for (auto& bus : tq.GetBuses()) {
            auto& name = bus.second->name_;
            bus_names.push_back(name);
        }       

        std::sort(bus_names.begin(), bus_names.end());

        return bus_names;
    }

    void  Renderer::AddBussesLine(transport_catalogue::TransportCatalogue& tq, svg::Document& document)
    {
        RenderSettings& settings = this->settings_;
        SphereProjector projector(geo_coordinates_.begin(),
            geo_coordinates_.end(), settings.width, settings.height, settings.padding);
        size_t color_position = 0;

        for (auto& bus : GetSortedBuses(tq)) {
            svg::Polyline bus_line;
            Bus* current_bus = tq.GetBus(bus);
            
            if (current_bus == nullptr) {
                continue;
            }

            bus_line.SetStrokeWidth(settings.line_width).SetFillColor("none");
            bus_line.SetStrokeColor(settings.colors[color_position]).SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            bus_line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            auto& stops = current_bus->bus_;

            for (auto& stop : stops) { 
                if ((stop->coordinates_.lat < 0) && (stop->coordinates_.lng < 0)) {
                    continue;
                }
                svg::Point screen_coordinates = projector(stop->coordinates_);
                bus_line.AddPoint(screen_coordinates);
            }


            if (color_position == settings.colors.size() - 1) {
                color_position = 0;
            }
            else {
                ++color_position;
            }

            document.Add(bus_line);
        }
    }

    void Renderer::UpDocumentTitle(svg::Document& document, Bus* current_bus, std::vector<geo::Coordinates>& stops_geo_coordinates, size_t color_position)
    {
        RenderSettings& settings = this->settings_;
        SphereProjector projector(geo_coordinates_.begin(),
            geo_coordinates_.end(), settings.width, settings.height, settings.padding);

        auto& stops = current_bus->bus_;
        auto stops_count = stops.size();


        if (stops_count == 0) {
            return;
        }

        svg::Text base_bus_title;
        svg::Text text_bus_title;
        SetStandBusBaseTextSettings(settings, base_bus_title);
        SetStandBusTextSettings(settings, text_bus_title);


        if (current_bus->is_round_trip_) {
            svg::Point first_end_stop_screen_coordinates = projector(stops[0]->coordinates_);
            base_bus_title.SetPosition(first_end_stop_screen_coordinates).SetData(current_bus->name_);
            text_bus_title.SetPosition(first_end_stop_screen_coordinates).SetData(current_bus->name_).SetFillColor(settings.colors[color_position]);
            document.Add(base_bus_title);
            document.Add(text_bus_title);
        }
        else {
            svg::Point first_end_stop_screen_coordinates = projector(stops[0]->coordinates_);

            base_bus_title.SetPosition(first_end_stop_screen_coordinates).SetData(current_bus->name_);
            text_bus_title.SetPosition(first_end_stop_screen_coordinates).SetData(current_bus->name_).SetFillColor(settings.colors[color_position]);

            document.Add(base_bus_title);
            document.Add(text_bus_title);

            if (stops[0] != stops[stops_geo_coordinates.size() / 2]) {

                svg::Point end_stop_screen_coordinates = projector(stops[stops_geo_coordinates.size() / 2]->coordinates_);
                svg::Text base_bus_title_beg;
                svg::Text text_bus_title_beg;

                SetStandBusBaseTextSettings(settings, base_bus_title_beg);
                SetStandBusTextSettings(settings, text_bus_title_beg);

                base_bus_title_beg.SetPosition(end_stop_screen_coordinates).SetData(current_bus->name_);
                text_bus_title_beg.SetPosition(end_stop_screen_coordinates).SetData(current_bus->name_).SetFillColor(settings.colors[color_position]);

                document.Add(base_bus_title_beg);
                document.Add(text_bus_title_beg);
            }
        }
    }

    void Renderer::AddBusTitle(transport_catalogue::TransportCatalogue& tq, svg::Document& document)
    {
        RenderSettings& settings = this->settings_;      
        size_t color_position = 0;       

        for (auto& bus : GetSortedBuses(tq)) {
            std::vector<geo::Coordinates> stops_geo_coordinates;
            Bus* current_bus = tq.GetBus(bus);

            for (auto& stop : current_bus->bus_) {
                geo::Coordinates coordinates;
                coordinates.lat = stop->coordinates_.lat;
                coordinates.lng = stop->coordinates_.lng;
                stops_geo_coordinates.push_back(coordinates);             
            }

            if (current_bus == nullptr) {
                continue;
            }

            auto& stops = current_bus->bus_;
            auto stops_count = stops.size();

            if (stops_count == 0) {
                continue;
            }

            UpDocumentTitle(document, current_bus, stops_geo_coordinates, color_position);   

            if (color_position == settings.colors.size() - 1) {
                color_position = 0;
            }
            else {
                ++color_position;
            }           
        }
    }

    void Renderer::AddBusCyrcle(transport_catalogue::TransportCatalogue& tq, svg::Document& document) {
        
        std::vector<std::string> stop_names;
        RenderSettings& settings = this->settings_;

        for (auto& stop : tq.GetStops()) {
            if ((stop.second->coordinates_.lat < 0) && (stop.second->coordinates_.lng < 0)) {
                continue;
            }
            if (stop.second->buses_.size() == 0) {
                continue;
            }
            stop_names.emplace_back(stop.second->name_);            
        }

        SphereProjector project(geo_coordinates_.begin(),
            geo_coordinates_.end(), settings.width, settings.height, settings.padding);

        std::sort(stop_names.begin(), stop_names.end());

        for (auto& stop : stop_names) {
            Stop* current_stop = tq.GetStop(stop);

            svg::Circle stop_cyrcle;
            svg::Point screen_coordinates = project(current_stop->coordinates_);

            stop_cyrcle.SetCenter(screen_coordinates).SetRadius(settings.stop_radius);

            stop_cyrcle.SetFillColor("white");
            document.Add(stop_cyrcle);
        }       
    } 

    void Renderer::SetStandBusBaseTextSettings(RenderSettings& rendset, svg::Text& txt)
    {
        txt.SetOffSet(rendset.bus_label_offset).SetFontSize(rendset.bus_label_font_size);
        txt.SetFontFamily("Verdana");
        txt.SetFontWeight("bold").SetStrokeWidth(rendset.underlayer_width);
        txt.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        txt.SetStrokeColor(rendset.underlayer_color).SetFillColor(rendset.underlayer_color);
    }
  
    void Renderer::SetStandBusTextSettings(RenderSettings& rendset, svg::Text& txt)
    {
        txt.SetOffSet(rendset.bus_label_offset).SetFontSize(rendset.bus_label_font_size);
        txt.SetFontFamily("Verdana");
        txt.SetFontWeight("bold")/*.SetStrokeWidth(rendset.underlayer_width)*/;       
    }

    void Renderer::SetStandStopSettings(RenderSettings& rendset, svg::Text& txt_base, svg::Text& txt_text)
    {
        txt_base.SetOffSet(rendset.stop_label_offset).SetFontSize(rendset.stop_label_font_size);
        txt_base.SetFontFamily("Verdana").SetFillColor(rendset.underlayer_color).SetStrokeColor(rendset.underlayer_color);
        txt_base.SetStrokeWidth(rendset.underlayer_width);
        txt_base.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        txt_text.SetOffSet(rendset.stop_label_offset).SetFontSize(rendset.stop_label_font_size);
        txt_text.SetFontFamily("Verdana").SetFillColor("black");       
    }  

    void Renderer::AddBusStopName(transport_catalogue::TransportCatalogue& tq, svg::Document& document)
    {        
        std::vector<std::string> stop_names;

        RenderSettings& settings = this->settings_;

        for (auto& stop : tq.GetStops()) {
            if ((stop.second->coordinates_.lat < 0) && (stop.second->coordinates_.lng < 0)) {
                continue;
            }
            if (stop.second->buses_.size() == 0) {
                continue;
            }
            stop_names.emplace_back(stop.second->name_);          
        }
        
        SphereProjector proj(geo_coordinates_.begin(),
            geo_coordinates_.end(), settings.width, settings.height, settings.padding);

        std::sort(stop_names.begin(), stop_names.end());

        for (auto& stop : stop_names) {
            Stop* current_stop = tq.GetStop(stop);
            svg::Text stop_name_text;
            svg::Text stop_name_base;

            SetStandStopSettings(settings, stop_name_base, stop_name_text);                       
            
            svg::Point screen_coordinates = proj(current_stop->coordinates_);

            stop_name_base.SetPosition(screen_coordinates).SetData(current_stop->name_);
            stop_name_text.SetPosition(screen_coordinates).SetData(current_stop->name_);

            document.Add(stop_name_base);
            document.Add(stop_name_text);
        }
    }

    void Renderer::PrintMap(transport_catalogue::TransportCatalogue& tq, std::ostream& output)
    {
       
        svg::Document& document = this->document_;
        AddBussesLine(tq, document);
        AddBusTitle(tq, document);
        AddBusCyrcle(tq, document);
        AddBusStopName(tq, document);

        document.Render(output);
    }

    void Renderer::AddSettings(RenderSettings& settings) 
    {
        this-> settings_ = settings;
    }

    void Renderer::AddCoordinates(transport_catalogue::TransportCatalogue& tq)
    {
        for (auto& bus : tq.GetBuses()) {

            for (auto& stop : bus.second->bus_) {
                if ((stop->coordinates_.lat < 0) && (stop->coordinates_.lng < 0)) {
                    continue;
                } 
                geo_coordinates_.emplace_back(stop->coordinates_);
            }
        }
    }

    void Renderer::PrintMapOut(transport_catalogue::TransportCatalogue& tq, std::ostream& output) 
    {
        
        svg::Document& document = this->document_;
        AddBussesLine(tq, document);
        AddBusTitle(tq, document);
        AddBusCyrcle(tq, document);
        AddBusStopName(tq, document);
        document.Render(output);
    }
}