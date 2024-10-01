#include "svg.h"
 
namespace svg {
 
using namespace std::literals;
 
void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    RenderObject(context);
    context.out << std::endl;
}
 
// ---------- Circle ------------------
 
Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}
 
Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}
 
void Circle::RenderObject(const RenderContext& context) const {
    std::ostream& out = context.out;
 
    out << "<circle cx=\""sv << center_.x 
        << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    
    RenderAttrs(context.out);
    out << "/>"sv;
}
 
// ---------- Polyline ------------------
    
Polyline& Polyline::AddPoint(Point point) {
    points_.emplace_back(point);
    return *this;
}
 
void Polyline::RenderObject(const RenderContext& context) const {
    
    std::ostream& out = context.out;
    out << "<polyline points=\"";
    
    for (size_t i = 0; i < points_.size(); ++i) {
        out << points_[i].x << ","sv << points_[i].y;
 
        if (i+1 != points_.size()) {
            out << " "sv;
        }
    }
    out << "\" "; 
    RenderAttrs(context.out);
    out << "/>";
}
 
// ---------- Text ------------------
    
Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}
 
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}
    
Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}
 
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
}
 
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
}
 
Text& Text::SetData(std::string data) {
    data_ = std::move(data);
    return *this;
}
 
std::string Text::delete_spaces(const std::string& str) {
    if (str.empty()) {
        return {};
    } else {
        
        auto left = str.find_first_not_of(' ');
        auto right = str.find_last_not_of(' ');
        return str.substr(left, right - left + 1);  
    }
}
 
std::string Text::uniq_symbols(const std::string& str) {
    
    std::string out;
 
    for (char ch : str) {
        
        if (ch == '"') {
            out += "&quot;"sv;
            continue;
            
        } else if (ch == '`' || ch == '\'') {
            out += "&apos;"sv;
            continue;
            
        } else if (ch == '<') {
            out += "&lt;"sv;
            continue;
            
        } else if (ch == '>') {
            out += "&gt;"sv;
            continue;
            
        } else if (ch == '&') {
            out += "&amp;"sv;
            continue;
            
        } else {
            
        }
        
        out += ch;
    }
 
    return out;
}
 
void Text::RenderObject(const RenderContext& context) const {
    
    std::ostream& out = context.out;
    out << "<text "; 
    RenderAttrs(context.out);
    out << "x=\"" 
        << position_.x << "\" y=\"" 
        << position_.y << "\" "
        << "dx=\"" 
        << offset_.x << "\" dy=\"" 
        << offset_.y << "\" "
        << "font-size=\"" 
        << font_size_ << "\" ";
 
    if (!font_family_.empty()) {
        out << "font-family=\"" << font_family_ << "\" ";
    }
 
    if (!font_weight_.empty()) {
        out << "font-weight=\"" << font_weight_ << "\"";
    }
 
    out << ">"sv << delete_spaces(uniq_symbols(data_)) << "</text>"sv;
 
}
 
// ---------- Document ------------------
 
void Document::Render(std::ostream& out) const {
    int indent = 2;
    int indent_step = 2;
 
    RenderContext context(out, indent_step, indent);
 
    const std::string_view xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv;
    const std::string_view svg = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv;
 
    out << xml << "\n"sv << svg << "\n"sv;
 
    for (const auto& object : objects_) {
        object->Render(context);
    }
    
    out << "</svg>"sv;
}
 
}//end namespace svg