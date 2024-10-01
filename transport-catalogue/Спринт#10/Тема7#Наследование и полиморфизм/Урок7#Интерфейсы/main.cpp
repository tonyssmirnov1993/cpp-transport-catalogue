#define _USE_MATH_DEFINES
#include "svg.h"
 
#include <cmath>
 
using namespace std::literals;
using namespace svg;
 
template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}
 
template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}
 
namespace shapes {
    
class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {}
 
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }
 
private:
    svg::Point p1_, p2_, p3_;
};
 
class Star : public svg::Drawable {
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
        polyline_ = CreateStar(center, 
                               outer_rad, 
                               inner_rad, 
                               num_rays);
    }
 
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(polyline_);
    }
 
private:
    
    svg::Polyline polyline_;
    
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
        using namespace svg;
        Polyline polyline;
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
 
class Snowman : public svg::Drawable {
public:
    Snowman(svg::Point head_point, double radius) : head_(head_point)
                                                  , radius_(radius){}
 
    void Draw(svg::ObjectContainer& container) const override {
        
        using namespace svg;
        Circle head;
        head.SetCenter(head_).SetRadius(radius_);
        
        Circle middle;
        middle.SetCenter({ head_.x, head_.y + radius_ * 2 }).SetRadius(radius_ * 1.5);
        
        Circle down;
        down.SetCenter({ head_.x, head_.y + radius_ * 5 }).SetRadius(radius_ * 2);
        
        container.Add(down);
        container.Add(middle);
        container.Add(head);
    }
    
private:
    svg::Point head_;
    double radius_;
};
 
}//end namespace shapes
 
int main() {
    using namespace svg;
    using namespace shapes;
    using namespace std;
 
    vector<unique_ptr<svg::Drawable>> picture;
 
    picture.emplace_back(make_unique<Triangle>(Point{100, 20}, Point{120, 50}, Point{80, 40}));
    picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
    picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));
 
    svg::Document doc;
    DrawPicture(picture, doc);
 
    doc.Render(cout);
}