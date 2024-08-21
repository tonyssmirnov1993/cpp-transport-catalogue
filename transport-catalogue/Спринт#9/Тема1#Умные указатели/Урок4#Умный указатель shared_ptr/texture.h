#pragma once
#include "common.h"

class Texture {
public:
    explicit Texture(Image image)
        : image_(std::move(image)) {
        //size_ = { int(image.size()), int(image[0].size())};
    }

    Size GetSize() const {
        // Заглушка. Реализуйте метод самостоятельно
        if (image_.empty()) {
            return { 0, 0 };
        }
        int hight = image_.size();
        int width = image_.back().size();
        return {width, hight};
    }

    char GetPixelColor(Point p) const {
        //(void) p;
        
       return image_[p.y][p.x];
    }

private:
    //Size size_ = {};
    Image image_;
};