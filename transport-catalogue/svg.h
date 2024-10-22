#pragma once
#include <fstream>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {

using namespace std::string_view_literals;

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    inline std::ostream& operator << (std::ostream& output, svg::StrokeLineCap cap) {
        if (cap == StrokeLineCap::BUTT) {
            output << "butt";
        }
        else if (cap == StrokeLineCap::ROUND) {
            output << "round";
        }
        else if (cap == StrokeLineCap::SQUARE) {
            output << "square";
        }
        else {}

        return output;
    }

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };
    inline std::ostream& operator<<(std::ostream& output, StrokeLineJoin join) {
        if (join == StrokeLineJoin::ARCS) {
            output << "arcs"sv;
        }
        else if (join == StrokeLineJoin::BEVEL) {
            output << "bevel"sv;
        }
        else if (join == StrokeLineJoin::MITER) {
            output << "miter"sv;
        }
        else if (join == StrokeLineJoin::MITER_CLIP) {
            output << "miter-clip"sv;
        }
        else if (join == StrokeLineJoin::ROUND) {
            output << "round"sv;
        }
        else {}

        return output;
    }
    //============== RGB ===================

    struct RGB
    {
        RGB() = default;
        RGB(uint8_t red, uint8_t green, uint8_t blue):
            red(red),
            green(green),
            blue(blue)
        {
        }
        
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };
    inline void PrintColor(std::ostream& output, RGB& rgb);
    //============== RGBA ================
    struct RGBA
    {
        RGBA(const uint8_t& red_, const uint8_t& green_, const uint8_t& blue_, double opacity_)
            : red(red_), green(green_), blue(blue_), opacity(opacity_)
        {
        }

        RGBA() = default;

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.0;
    };
    inline void PrintColor(std::ostream& output, RGBA& rgba);        

    using Color = std::variant<std::monostate, std::string, RGB, RGBA>;

    inline const Color NoneColor{ "none" };
    inline void PrintColor(std::ostream& output, std::monostate);
    inline void PrintColor(std::ostream& output, std::string &color);

    std::ostream& operator<<(std::ostream& output, const Color& color);

    //================= PathProps ======================
    template <typename Owner>
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
            width_ = width;
            return AsOwner();
        }
        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = line_cap;
            return AsOwner();
        }
        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        // Метод RenderAttrs выводит в поток общие для всех путей атрибуты fill и stroke
        void RenderAttrs(std::ostream& output) const {
            using namespace std::literals;

            if (fill_color_ != std::nullopt) {
                output << "fill=\""sv << fill_color_.value() << "\" "sv;
            }
            if (stroke_color_ != std::nullopt) {
                output << "stroke=\""sv << stroke_color_.value() << "\" "sv;
            }
            if (width_ != std::nullopt) {
                output << "stroke-width=\""sv << width_.value() << "\" "sv;
            }
            if (line_cap_ != std::nullopt) {
                output << "stroke-linecap=\""sv << line_cap_.value() << "\" "sv;
            }
            if (line_join_ != std::nullopt) {
                output << "stroke-linejoin=\""sv << line_join_.value() << "\" "sv;
            }
        }

    private:
        Owner& AsOwner() {
            // static_cast безопасно преобразует *this к Owner&, если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> width_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;
    };

    struct Point {
        Point() = default;
        Point(double x, double y): 
            x(x)
           ,y(y) {
        }
        double x = 0;
        double y = 0;
    };

/*
 Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& output): 
        output(output) {
    }

    RenderContext(std::ostream& output, int indent_step, int indent = 0):
        output(output)
       ,indent_step(indent_step)
       ,indent(indent) {
    }

    RenderContext Indented() const {
        return { output, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            output.put(' ');
        }
    }

    std::ostream& output;
    int indent_step = 0;
    int indent = 0;
};

/*
 Абстрактный базовый класс Object служит для унифицированного хранения
 конкретных тегов SVG-документа
 Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object  {

public:

    void Render(const RenderContext& context) const;
    virtual ~Object() = default;

private:

    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 Класс Circle моделирует элемент <circle> для отображения круга
 https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */

class Circle final : public Object, public PathProps<Circle> {

public:

    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:

    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline : public Object, public PathProps<Polyline> {
public:


    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);
    void RenderObject(const RenderContext& context) const override;
   
private:
    Point pol_;
    std::vector<Point> vec_pol_;
};

/*
Класс Text моделирует элемент <text> для отображения текста
 https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text : public Object, public PathProps<Text> {

public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffSet(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

private:

    std::string font_family_;
    std::string font_weight_;
    std::string data_;
    uint32_t font_size_ = 1;
    Point offset_;
    Point position_;

    static std::string DeleteSpaces(const std::string& data);
    static std::string UniqSymbols(const std::string data);
    void RenderObject(const RenderContext& context) const override;

    // Прочие данные и методы, необходимые для реализации элемента <text>
};

class ObjectContainer {

public:
    //Метод Add добавляет в svg-документ любой объект-наследник svg::Object.    
    template<typename Object>
    void Add(Object obj);

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    virtual ~ObjectContainer() = default;

    std::vector< std::unique_ptr<Object>> objects_;
};

template<typename Object>

void ObjectContainer::Add(Object obj) {
    objects_.emplace_back(std::make_unique<Object>(std::move(obj)));
}

class Drawable {

public:

    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};

class Document : public ObjectContainer{

public:
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj);    
    // Выводит в ostream svg-представление документа
    void Render(std::ostream& output) const;  
};
}// namespace svg