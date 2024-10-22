#pragma once
#include "geo.h"
#include "svg.h"
#include "transport_catalogue.h"
#include <algorithm>
#include <cstdlib>
#include <optional>
#include <vector>



using namespace transport_catalogue;

namespace map_renderer {

    inline const double EPSILON = 1E-6;

    struct RenderSettings{

        double width=0.0;
        double height = 0.0;
        double padding=0.0;
        double line_width=0.0;
        double stop_radius = 0.0;
        double underlayer_width = 0;
        int bus_label_font_size = 0;
        int stop_label_font_size = 0;

        svg::Point bus_label_offset{0,0};
        svg::Point stop_label_offset{ 0,0 };       
        svg::Color underlayer_color;
        std::vector<svg::Color> colors{};     
    };

    class Renderer {

    public:     
        Renderer() = default;
        void PrintMap(transport_catalogue::TransportCatalogue& tq, std::ostream& output);
        void AddSettings(RenderSettings& settings);
        void AddCoordinates(transport_catalogue::TransportCatalogue& tq);
        void PrintMapOut(transport_catalogue::TransportCatalogue& tq, std::ostream& output);
        
    private:
        //часть методов перенесена в приватную часть
        std::vector<std::string> GetSortedBuses(transport_catalogue::TransportCatalogue& tq);
        void AddBussesLine(transport_catalogue::TransportCatalogue& tq, svg::Document& document);
        void UpDocumentTitle(svg::Document& document, Bus* current_bus, std::vector<geo::Coordinates> &stops_geo_coordinates, size_t color_position);
        void AddBusTitle(transport_catalogue::TransportCatalogue& tq, svg::Document& document);
        void AddBusCyrcle(transport_catalogue::TransportCatalogue& tq, svg::Document& document);
        void AddBusStopName(transport_catalogue::TransportCatalogue& tq, svg::Document& document);
        void SetStandBusBaseTextSettings(RenderSettings& rendset, svg::Text& txt);
        void SetStandBusTextSettings(RenderSettings& rendset, svg::Text& txt);
        void SetStandStopSettings(RenderSettings& rendset, svg::Text& txt_base, svg::Text& txt_text);
       

        RenderSettings settings_;
        svg::Document document_;
        std::vector<geo::Coordinates> geo_coordinates_;       
    };
   
    class SphereProjector {

    public:
        template <typename PointInputIt>

        SphereProjector(PointInputIt points_begin, PointInputIt points_end,

            double max_width, double max_height, double padding): 
            padding_(padding)
        {
            if (points_begin == points_end) {
                return;
            }

            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_long_ = left_it->lng;
            const double max_long = right_it->lng;

            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            std::optional<double> width_zoom;
            if (!IsZero(max_long - min_long_)) {
                width_zoom = (max_width - 2 * padding) / (max_long - min_long_);
            }

            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                zoom_koeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom) {
                zoom_koeff_ = *width_zoom;
            }
            else if (height_zoom) {
                zoom_koeff_ = *height_zoom;
            }
        }

        svg::Point operator()(geo::Coordinates coords) const {
            return {
                (coords.lng - min_long_) * zoom_koeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_koeff_ + padding_
            };
        }
        bool IsZero(double value) {
            return std::abs(value) < EPSILON;
        }

    private:

        double padding_;
        double min_long_ = 0;
        double max_lat_ = 0;
        double zoom_koeff_ = 0;
    };
}