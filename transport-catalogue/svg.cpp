#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream& operator<<(std::ostream& output, Color& color) {
        std::visit(ColorPrinter{ output }, color);
        return output;
    }
    std::ostream& operator<<(std::ostream& output, StrokeLineCap line_cap) {
        switch (line_cap) {
        case StrokeLineCap::BUTT:
            output << "butt"sv;
            break;
        case StrokeLineCap::ROUND:
            output << "round"sv;
            break;
        case StrokeLineCap::SQUARE:
            output << "square"sv;
            break;
        }
        return output;
    }

    std::ostream& operator<<(std::ostream& output, StrokeLineJoin line_join) {
        switch (line_join) {
        case StrokeLineJoin::ARCS:
            output << "arcs"sv;
            break;
        case StrokeLineJoin::BEVEL:
            output << "bevel"sv;
            break;
        case StrokeLineJoin::MITER:
            output << "miter"sv;
            break;
        case StrokeLineJoin::MITER_CLIP:
            output << "miter-clip"sv;
            break;
        case StrokeLineJoin::ROUND:
            output << "round"sv;
            break;
        }
        return output;
    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // ƒелегируем вывод тега своим подклассам
        RenderObject(context);

        context.output << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& output = context.output;
        output << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        output << "r=\""sv << radius_ << "\""sv;
        // ¬ыводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.output);
        output << "/>"sv;
    }

    // ---------- Polyline ----------------

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(std::move(point));
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& output = context.output;
        output << "<polyline points=\""sv;
        bool is_first = true;
        for (auto& point : points_) {
            if (is_first) {
                output << point.x << "," << point.y;
                is_first = false;
            }
            else {
                output << " "sv << point.x << "," << point.y;
            }
        }
        output << "\"";
        // ¬ыводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.output);
        output << "/>"sv;
    }

    // ---------- Text --------------------

    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        size_ = size;
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

    void Text::RenderObject(const RenderContext& context) const {
        auto& output = context.output;
        output << "<text";
        // ¬ыводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.output);
        output << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
        output << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
        output << "font-size=\""sv << size_ << "\""sv;
        if (!font_family_.empty()) output << " font-family=\""sv << font_family_ << "\" "sv;
        if (!font_weight_.empty()) output << "font-weight=\""sv << font_weight_ << "\""sv;
        output << ">"sv << data_ << "</text>"sv;
    }

    // ---------- Document ----------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& output) const {
        RenderContext ctx(output, 2, 2);
        output << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        output << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& obj : objects_) {
            obj->Render(ctx);
        }
        output << "</svg>"sv;
    }
} // namespace svg