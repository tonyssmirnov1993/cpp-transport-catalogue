#pragma once
#include <array>
#include <cassert>
#include <vector>
#include "geom.h"
#include <string>
class Tile {
public:
    // Размер тайла 8*8 пикселей.
    constexpr static int SIZE = 8;
 
    // Конструктор по умолчанию. Заполняет тайл указанным цветом.
    Tile(char color = ' ') noexcept {
        /* Реализуйте недостающий код самостоятельно. */
        std::fill(&pixels_[0][0],&pixels_[0][0] + SIZE * SIZE, color);
        assert(instance_count_ >= 0);
        ++instance_count_;  // Увеличиваем счётчик тайлов (для целей тестирования).
    }
 
    Tile(const Tile& other) {
        /* Реализуйте недостающий код самостоятельно. */
        pixels_ = other.pixels_;
        assert(instance_count_ >= 0);
        ++instance_count_;  // Увеличиваем счётчик тайлов (для целей тестирования).
    }
 
    ~Tile() {
        --instance_count_;  // Уменьшаем счётчик тайлов.
        assert(instance_count_ >= 0);
    }
 
    /**
     * Изменяет цвет пикселя тайла.
     * Если координаты выходят за пределы тайла, метод ничего не делает.
     */
    void SetPixel(Point p, char color) noexcept {
        /* Реализуйте недостающий код самостоятельно. */
 
        if ((p.x < SIZE || p.y < SIZE) && p.x >= 0 && p.y >= 0) {
            pixels_[p.x][p.y] = color;
        }
    }
 
    /**
     * Возвращает цвет пикселя. Если координаты выходят за пределы тайла, возвращается пробел.
     */
 
 
    char GetPixel(Point p) const noexcept {
        /* Реализуйте недостающий функционал самостоятельно. */
        if (p.x >= SIZE || p.y >= SIZE || p.x < 0 || p.y < 0) { 
            return ' '; 
        }
        return  pixels_[p.x][p.y]; 
    }
 
    // Возвращает количество экземпляра класса Tile в программе.
    static int GetInstanceCount() noexcept {
        return instance_count_;
    }
 
private:
    inline static int instance_count_ = 0;
 
    /* Разместите здесь поля для хранения пикселей тайла. */
    std::array<std::array<char, SIZE>, SIZE> pixels_;
};