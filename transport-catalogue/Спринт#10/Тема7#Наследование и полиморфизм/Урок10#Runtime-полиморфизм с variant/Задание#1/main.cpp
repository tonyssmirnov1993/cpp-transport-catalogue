#include "svg.h"
#include <cassert>
 
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
 
int main() {
    using namespace svg;
    using namespace std;
 
    Color none_color;
    cout << none_color << endl;
    
    Color purple{"purple"s};
    cout << purple << endl;
    
    Color rgb = Rgb{100, 200, 255};
    cout << rgb << endl;
 
    Color rgba = Rgba{100, 200, 255, 0.5};
    cout << rgba << endl;
 
    Circle c;
    c.SetRadius(3.5).SetCenter({1.0, 2.0});
    c.SetFillColor(rgba);
    c.SetStrokeColor(purple);
    
    Document doc;
    doc.Add(std::move(c));
    doc.Render(cout);
}