#pragma once
#define _USE_MATH_DEFINES
 
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <cmath>
 
namespace svg {
    
using Color = std::string;
inline const Color NoneColor{"none"};
 
enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};
    
inline std::ostream &operator<<(std::ostream &out, StrokeLineCap stroke_line_cap) {
    using namespace std::literals;
    if (stroke_line_cap == StrokeLineCap::BUTT) {
        out << "butt"sv;
    } else if (stroke_line_cap == StrokeLineCap::ROUND) {
        out << "round"sv;
    } else if (stroke_line_cap == StrokeLineCap::SQUARE) {
        out << "square"sv;
    }
    return out;
}
 
enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};
    
inline std::ostream &operator<<(std::ostream &out, StrokeLineJoin stroke_line_join) {
    using namespace std::literals;
    if (stroke_line_join == StrokeLineJoin::ARCS) {
        out << "arcs"sv;
    } else if (stroke_line_join == StrokeLineJoin::BEVEL) {
        out << "bevel"sv;
    } else if (stroke_line_join == StrokeLineJoin::MITER) {
        out << "miter"sv;
    } else if (stroke_line_join == StrokeLineJoin::MITER_CLIP) {
        out << "miter-clip"sv;
    } else if (stroke_line_join == StrokeLineJoin::ROUND) {
        out << "round"sv;
    }
    return out;
}
    
template<typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
 
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
 
    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }
 
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_line_cap_ = line_cap;
        return AsOwner();
    }
 
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_line_join_ = line_join;
        return AsOwner();
    } 
 
protected:
    ~PathProps() = default;
 
    void RenderAttrs(std::ostream &out) const {
        using namespace std::literals;
 
        if (fill_color_ != std::nullopt) {
            out << "fill=\""sv << fill_color_.value() << "\" "sv;
        }
        if (stroke_color_ != std::nullopt) {
            out << "stroke=\""sv << stroke_color_.value() << "\" "sv;
        }
        if (stroke_width_ != std::nullopt) {
            out << "stroke-width=\""sv << stroke_width_.value() << "\" "sv;
        }
        if (stroke_line_cap_ != std::nullopt) {
            out << "stroke-linecap=\""sv << stroke_line_cap_.value() << "\" "sv;
        }
        if (stroke_line_join_ != std::nullopt) {
            out << "stroke-linejoin=\""sv << stroke_line_join_.value() << "\" "sv;
        }
    }
    
private: 
    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_line_cap_;
    std::optional<StrokeLineJoin> stroke_line_join_;
    
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }
};
    
struct Point {
    Point() = default;
    Point(double x, double y) : x(x)
                              , y(y) {}
    double x = 0;
    double y = 0;
};
 
struct RenderContext {
    RenderContext(std::ostream& out) : out(out) {}
 
    RenderContext(std::ostream& out, int indent_step, int indent = 0) : out(out)
                                                                      , indent_step(indent_step)
                                                                      , indent(indent) {}
 
    RenderContext Indented() const {
        return {out, 
                indent_step, 
                indent + indent_step};
    }
 
    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }
 
    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};
 
class Object {
public:
    void Render(const RenderContext& context) const;
    virtual ~Object() = default;
 
private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};
 
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);
 
private:
    Point center_;
    double radius_ = 1.0;
    
    void RenderObject(const RenderContext& context) const override;
};
 
class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);
private:
    std::vector<Point> points_;
    void RenderObject(const RenderContext& context) const override;
};
 
class Text final : public Object, public PathProps<Text> {
public:
    Text& SetPosition(Point pos);
    Text& SetOffset(Point offset);
    Text& SetFontSize(uint32_t size);
    Text& SetFontFamily(std::string font_family);
    Text& SetFontWeight(std::string font_weight);
    Text& SetData(std::string data);
 
private:
    Point position_;
    Point offset_;
    std::string font_family_;
    std::string font_weight_;
    uint32_t font_size_ = 1;
    std::string data_;
     
    static std::string delete_spaces(const std::string& str);
    static std::string uniq_symbols(const std::string& str);
    
    void RenderObject(const RenderContext& context) const override;
};
    
class ObjectContainer {
public:
    virtual ~ObjectContainer() = default;
 
    template<typename Obj>
    void Add(Obj obj);
 
    virtual void AddPtr(std::unique_ptr<Object> &&) = 0;
 
protected:
    std::vector<std::unique_ptr<Object>> objects_;
};
    
template<typename Obj>
void ObjectContainer::Add(Obj obj) {
    objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
}
    
class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};
 
namespace shapes {   
class Triangle : public Drawable {
public:
    Triangle(Point p1, Point p2, Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {}
 
    void Draw(ObjectContainer& container) const override {
        container.Add(Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }
 
private:
    Point p1_, p2_, p3_;
};
 
class Star : public Drawable {
public:
    Star(Point center, double outer_rad, double inner_rad, int num_rays) {
        polyline_ = CreateStar(center, 
                               outer_rad, 
                               inner_rad, 
                               num_rays);
    }
 
    void Draw(ObjectContainer& container) const override {
        container.Add(polyline_);
    }
 
private:
    
    Polyline polyline_;
    
    Polyline CreateStar(Point center, double outer_rad, double inner_rad, int num_rays) {
        using namespace svg;
        Polyline polyline;
        polyline.SetFillColor("red");
        polyline.SetStrokeColor("black");
        for (int i = 0; i <= num_rays; ++i) {
            double angle = 2 * M_PI * (i % num_rays) / num_rays;
            polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
            if (i == num_rays) {
                break;
            }
            angle += M_PI / num_rays;
            polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
            }
        return polyline;
    }
};
 
class Snowman : public Drawable {
public:
    Snowman(Point head_point, double radius) : head_(head_point)
                                             , radius_(radius){}
 
    void Draw(ObjectContainer& container) const override {
        
        using namespace svg;
        Circle head;
        head.SetFillColor("rgb(240,240,240)");
        head.SetStrokeColor("black");
        head.SetCenter(head_).SetRadius(radius_);
        
        Circle middle;
        middle.SetFillColor("rgb(240,240,240)");
        middle.SetStrokeColor("black");
        middle.SetCenter({ head_.x, head_.y + radius_ * 2 }).SetRadius(radius_ * 1.5);
        
        Circle down;
        down.SetFillColor("rgb(240,240,240)");
        down.SetStrokeColor("black");
        down.SetCenter({ head_.x, head_.y + radius_ * 5 }).SetRadius(radius_ * 2);
        
        container.Add(down);
        container.Add(middle);
        container.Add(head);
    }
    
private:
    Point head_;
    double radius_;
}; 
}//end namespace shapes
    
class Document : public ObjectContainer {
public:
    
    void AddPtr(std::unique_ptr<Object>&& obj) override {
        objects_.emplace_back(std::move(obj));
    }
    void Render(std::ostream& out) const;
};
 
}//end namespace svg