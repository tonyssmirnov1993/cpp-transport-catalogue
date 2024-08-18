#pragma once
#include "texture.h"
 
#include <memory>
 
// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType { RECTANGLE, ELLIPSE};
 
class Shape {
public:
    // Фигура после создания имеет нулевые координаты и размер,
    // а также не имеет текстуры
    explicit Shape(ShapeType type):
    type_(type)    
    {
    }
 
    void SetPosition(Point pos) {
        position_ = pos;
    }
 
    void SetSize(Size size) {
        size_ = size;
    }
 
    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = texture;
    }
 
    // Рисует фигуру на указанном изображении
    // В зависимости от типа фигуры должен рисоваться либо эллипс, либо прямоугольник
    // Пиксели фигуры, выходящие за пределы текстуры, а также в случае, когда текстура не задана,
    // должны отображаться с помощью символа точка '.'
    // Части фигуры, выходящие за границы объекта image, должны отбрасываться.
    void Draw(Image& image) const {
        Size texture_size = {0, 0};
        if (texture_){
            texture_size = texture_->GetSize();
        }
        for (int y = 0; y < size_.height; ++y){
            for (int x = 0; x < size_.width; ++x){
                if (((type_ == ShapeType::ELLIPSE) && IsPointInEllipse({x,y}, size_)) || (type_ == ShapeType::RECTANGLE)){
                    if ((x < texture_size.width) && (y < texture_size.height)){
                        image[y + position_.y][x + position_.x] = texture_->GetPixelColor({x,y});
                    } else {
                        image[y + position_.y][x + position_.x] = '.';
                    }
                }
            }
        }
    }
    
private:
 
Size size_ = {0,0};
Point position_ = {0,0};
ShapeType type_ = ShapeType::RECTANGLE;
std::shared_ptr<Texture> texture_ = nullptr;
};