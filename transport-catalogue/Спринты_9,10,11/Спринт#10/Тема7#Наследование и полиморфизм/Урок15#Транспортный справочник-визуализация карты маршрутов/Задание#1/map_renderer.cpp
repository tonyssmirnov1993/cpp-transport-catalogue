#include "map_renderer.h"
 
namespace map_renderer {
 
bool SphereProjector::is_zero(double value) {
    return std::abs(value) < EPSILON;
}
    
MapRenderer::MapRenderer(RenderSettings& render_settings) : render_settings_(render_settings) {}
 
svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
    return {(coords.longitude - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.latitude) * zoom_coeff_ + padding_};
}
    
SphereProjector MapRenderer::get_sphere_projector(const std::vector<geo::Coordinates>& points) const {
        return SphereProjector(points.begin(),
                               points.end(),
                               render_settings_.width_,
                               render_settings_.height_,
                               render_settings_.padding_);
}
    
RenderSettings MapRenderer::get_render_settings() const{
    return render_settings_;
}
  
int MapRenderer::get_palette_size() const{
    return render_settings_.color_palette_.size();
}
    
svg::Color MapRenderer::get_color(int line_number) const {
    return render_settings_.color_palette_[line_number];
}
 
void MapRenderer::set_line_properties(svg::Polyline& polyline, [[maybe_unused]] int line_number) const {
    using namespace std::literals;
    
    polyline.set_stroke_color(get_color(line_number));   
    polyline.set_fill_color("none"s);
    polyline.set_stroke_width(render_settings_.line_width_);
    polyline.set_stroke_linecap(svg::StrokeLineCap::ROUND);
    polyline.set_stroke_linejoin(svg::StrokeLineJoin::ROUND);   
}   
    
}//end namespace map_renderer