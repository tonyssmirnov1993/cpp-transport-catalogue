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
    
    out << R"(<circle cx=")" 
        << center_.x << R"(" cy=")" 
        << center_.y << R"(" )" 
        << R"(r=")" 
        << radius_ << R"(" )" << "/>"sv;
}
 
// ---------- Polyline ------------------
    
Polyline& Polyline::AddPoint(Point point) {
    points_.emplace_back(point);
    return *this;
}
 
void Polyline::RenderObject(const RenderContext& context) const {
    
    std::ostream& out = context.out;
    out << R"(<polyline points=")";
 
    for (size_t i = 0; i < points_.size(); ++i) {
        out << points_[i].x << ","sv << points_[i].y;
 
        if (i+1 != points_.size()) {
            out << " "sv;
        }
    }
 
    out << R"(")" << " />"sv;
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
            
        } else {}
        
        out += ch;
    }
 
    return out;
}
 
void Text::RenderObject(const RenderContext& context) const {
    
    std::ostream& out = context.out;
 
    out << R"(<text x=")" 
        << position_.x << R"(" y=")" 
        << position_.y << R"(" )"
        << R"(dx=")" 
        << offset_.x << R"(" dy=")" 
        << offset_.y << R"(" )"
        << R"(font-size=")" 
        << font_size_ << R"(")";
 
    if (!font_family_.empty()) {
        out << R"( font-family=")" << font_family_ << R"(")";
    }
 
    if (!font_weight_.empty()) {
        out << R"( font-weight=")" << font_weight_ << R"(")";
    }
 
    out << ">"sv << delete_spaces(uniq_symbols(data_)) << "</text>"sv;
    
}
 
// ---------- Document ------------------
    
void Document::AddPtr(std::unique_ptr<Object> &&obj) {
    objects_.emplace_back(std::move(obj));
}
 
void Document::Render(std::ostream &out) const {
    int indent = 2;
    int indent_step = 2;
 
    RenderContext context(out, indent_step, indent);
 
    const std::string_view xml = R"(<?xml version="1.0" encoding="UTF-8" ?>)";
    const std::string_view svg = R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)";
 
    out << xml << "\n"sv << svg << "\n"sv;
 
    for (const auto& object : objects_) {
        object->Render(context);
    }
    
    out << "</svg>"sv;
}
 
}  // namespace svg