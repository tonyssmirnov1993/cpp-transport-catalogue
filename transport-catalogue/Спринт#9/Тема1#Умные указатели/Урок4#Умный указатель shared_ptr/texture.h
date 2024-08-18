#pragma once
#include "common.h"
 
#include <iostream>
 
class Texture {
public:
    explicit Texture(Image image)
        : image_(std::move(image)) {
    }
 
    Size GetSize() const {
        int height = image_.size();
        int width = image_.back().size();
        return {width, height};
    }
 
    char GetPixelColor(Point p) const {
        return image_[p.y][p.x];
    }
 
private:
    Image image_;
};