#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.output << std::endl;
}

// =============== Circle ========================

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& output = context.output;
    output << R"(<circle cx=")"sv << center_.x
        << R"(" cy=")"sv << center_.y << R"(" )";
    output << R"(r=")"sv << radius_ << R"(" )";
    RenderAttrs(context.output);
    output << "/>"sv;
}

Polyline& Polyline::AddPoint(Point point)
{
    vec_pol_.push_back(point);
    return *this;   
}

//============== Polyline =========================
void Polyline::RenderObject(const RenderContext& context) const
{
    auto& output = context.output;
    output << R"(<polyline points=")";
    for (size_t i = 0; i < vec_pol_.size(); i++)
    {
        output << vec_pol_[i].x << ","sv << vec_pol_[i].y;
        if (i + 1 != vec_pol_.size()) {
            output << " "sv;
        }
    }
    output << R"(" )";
    RenderAttrs(context.output);
    /*out << R"(")" << " />"sv;*/
    output << "/>";
}
//=================== Text ==========================
Text& Text::SetPosition(Point position)
{
    position_ = position;
    return *this;   
}

Text& Text::SetOffset(Point offset)
{
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size)
{
    font_size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family)
{
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight)
{
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(std::string data)
{
    data_ = data;
    return *this;
}

std::string Text::DeleteSpaces(const std::string& data)
{
    if (data.empty()) {
        return{};
    }

    auto left = data.find_first_not_of(' ');
    auto right = data.find_last_not_of(' ');

    return data.substr(left, right - left + 1);  
}

std::string Text::UniqSymbols(const std::string data)
{
    std::string output;
    for (auto ch : data) {
        if (ch == '"') {
            output += "&quot;"sv;
            continue;
        }
        else if (ch == '`' || ch == '\'') {
            output += "&apos;";
            continue;
        }
        else if (ch == '<') {
            output += "&lt;";
            continue;
        }
        else if (ch == '>') {
            output += "&gt:";
            continue;
        }
        else if (ch == '&') {
            output += "&amp;";
            continue;
        }else{}
        output += ch;
    }
    return output;
}

void Text::RenderObject(const RenderContext& context) const
{
    auto& output = context.output;
    output << "<text ";
    RenderAttrs(context.output);
    output << " x=\""

        << position_.x <<"\" y=\""
        << position_.y<<"\" dx=\""
        << offset_.x<<"\" dy=\""
        << offset_.y<<"\" font-size=\""
        << font_size_ << "\"";

    if (!font_family_.empty()) {
        output << " font-family=\"" << font_family_<<"\"";
    }
    if (!font_weight_.empty()) {
        output <<" font-weight=\""<< font_weight_<<"\"";
    }
    output << ">"sv<< DeleteSpaces( UniqSymbols(data_))<<"</text>";
}

void Document::AddPtr(std::unique_ptr<Object>&& obj)
{
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream& output) const
{
    int indent_step = 2;
    int indent = 2;
    std::string_view xml = R"(<?xml version="1.0" encoding="UTF-8" ?>)";
    std::string_view svg = R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)";
    RenderContext context(output, indent_step, indent);
    output << xml << "\n"sv << svg << "\n"s;
    for (auto &obj : objects_) {
        obj.get()->Render(context);
    }
    output << "</svg>"sv;
}

void PrintColor(std::ostream& output, RGB& rgb)
{
    output << "rgb("sv << static_cast<short>(rgb.red) << ","sv
        << static_cast<short>(rgb.green) << ","sv
        << static_cast<short>(rgb.blue) << ")"sv;    
}

void PrintColor(std::ostream& output, RGBA& rgba)
{
    output << "rgba("sv << static_cast<short>(rgba.red) << ","sv
        << static_cast<short>(rgba.green) << ","sv
        << static_cast<short>(rgba.blue) << ","sv
        << (rgba.opacity) << ")"sv;   
}

void PrintColor(std::ostream& output, std::monostate)
{
    output << "none"sv;
}

void PrintColor(std::ostream& output, std::string& color)
{ 
    output << color;
}

std::ostream& operator<<(std::ostream& output, const Color& color)
{
    std::visit([&output](auto value) {PrintColor(output, value); }, color);
    return output;
}

}// namespace svg