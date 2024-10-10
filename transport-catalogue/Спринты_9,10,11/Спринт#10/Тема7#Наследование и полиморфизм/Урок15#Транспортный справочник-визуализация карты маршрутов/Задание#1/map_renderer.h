#pragma once
#include <iostream>
#include <optional>
#include <algorithm>
#include <cstdlib>
 
#include "domain.h"
#include "geo.h"
#include "svg.h"
 
using namespace transport_catalogue::detail;
 
namespace map_renderer {
    
inline const double EPSILON = 1e-6;
 
class SphereProjector {
public:
 
    template <typename InputIt>
    SphereProjector(InputIt points_begin, 
                    InputIt points_end,
                    double max_width, 
                    double max_height, 
                    double padding);
 
    svg::Point operator()(geo::Coordinates coords) const;
 
private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
    
    bool is_zero(double value);
};
    
struct RenderSettings {
    double width_;
    double height_;
    double padding_;
    double line_width_;
    double stop_radius_;
    int bus_label_font_size_;
    std::pair<double, double> bus_label_offset_;
    int stop_label_font_size_;
    std::pair<double, double>  stop_label_offset_;
    svg::Color underlayer_color_;
    double underlayer_width_;
    std::vector<svg::Color> color_palette_;
};
    
class MapRenderer {
public:
    MapRenderer(RenderSettings& render_settings);   
    SphereProjector get_sphere_projector(const std::vector<geo::Coordinates>& points) const;
    
    RenderSettings get_render_settings() const;
    int get_palette_size() const;
    svg::Color get_color(int line_number) const;
        
    void set_line_properties(svg::Polyline& polyline, int line_number) const; 
 
private:
    RenderSettings& render_settings_;
};
 
template <typename InputIt>
    SphereProjector::SphereProjector(InputIt points_begin, 
                                     InputIt points_end,
                                     double max_width, 
                                     double max_height, 
                                     double padding) : padding_(padding) {
        if (points_begin == points_end) {
            return;
        }
 
        const auto [left_it, 
                    right_it] = std::minmax_element(points_begin, 
                                                    points_end,
            [](auto lhs, auto rhs) { 
                return lhs.longitude < rhs.longitude;
            });
        
        min_lon_ = left_it->longitude;
        const double max_lon = right_it->longitude;
 
        const auto [bottom_it, 
                    top_it] = std::minmax_element(points_begin, 
                                                  points_end,
            [](auto lhs, auto rhs) { 
                return lhs.latitude < rhs.latitude;
            });
        
        const double min_lat = bottom_it->latitude;
        max_lat_ = top_it->latitude;
 
        std::optional<double> width_zoom;
        if (!is_zero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) 
                         / (max_lon - min_lon_);
        }
 
        std::optional<double> height_zoom;
        if (!is_zero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) 
                          / (max_lat_ - min_lat);
        }
 
        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom, 
                                   *height_zoom);     
        } else if (width_zoom) {
            zoom_coeff_ = *width_zoom;
            
        } else if (height_zoom) {
            zoom_coeff_ = *height_zoom;
            
        }
    }
    
}//end namespace map_renderer