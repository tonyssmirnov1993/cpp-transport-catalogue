#pragma once
#include "texture.h"

#include <memory>

// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType { RECTANGLE, ELLIPSE};

class Shape {
public:
    // Фигура после создания имеет нулевые координаты и размер,
    // а также не имеет текстуры
    explicit Shape(ShapeType type) {
        // Заглушка. Реализуйте конструктор самостоятельно
        this->type_ = type;
        
        
    }

    void SetPosition(Point pos) {
        this->pos_ = pos;
        
        // Заглушка. Реализуйте метод самостоятельно
    }

    void SetSize(Size size) {
        this->size_ = size;
        //(void) size;
        // Заглушка. Реализуйте метод самостоятельно
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = texture;
        
        // Заглушка. Реализуйте метод самостоятельно
    }

    // Рисует фигуру на указанном изображении
	// В зависимости от типа фигуры должен рисоваться либо эллипс, либо прямоугольник
    // Пиксели фигуры, выходящие за пределы текстуры, а также в случае, когда текстура не задана,
    // должны отображаться с помощью символа точка '.'
    // Части фигуры, выходящие за границы объекта image, должны отбрасываться.
    void Draw(Image& image) const {
        Size texture_size = { 0, 0 };

        const auto image_size = GetImageSize(image); //

        if (texture_) {
            texture_size = texture_->GetSize();
        }
        
       
        for (int y = 0; y < size_.height; ++y) {
            for (int x = 0; x < size_.width; ++x) {
               

                if ((type_ == ShapeType::ELLIPSE && IsPointInEllipse({ x,y }, size_)) || type_ == ShapeType::RECTANGLE) {
                    
                    if ((pos_.y + y) >= image_size.height ||
                        (pos_.x + x) >= image_size.width) {
                        continue;
                    }

                    if ((x < texture_size.width) && (y < texture_size.height)) {
                        image[pos_.y + y][pos_.x + x] = texture_.get()->GetPixelColor({ x,y });
                    }
                    else {
                        image[pos_.y + y][pos_.x + x] = '.';
                    }
                   
                }
                
            }
        }
         
        // Заглушка. Реализуйте метод самостоятельно
    }
private:
    ShapeType type_ = ShapeType::RECTANGLE;
    Point pos_ = { 0,0 };
    
    Size size_ = { 0,0 };
    std::shared_ptr<Texture> texture_ = nullptr;
};