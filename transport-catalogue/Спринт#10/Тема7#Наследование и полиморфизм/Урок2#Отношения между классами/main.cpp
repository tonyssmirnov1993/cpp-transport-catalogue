#include <iostream>
#include <cassert>
#include <string>
 
using namespace std;
 
class House {
public:
    explicit House(int length, int width, int heigth) : length_(length),
                                                        width_(width),
                                                        height_(heigth) {}
 
    int GetLength() const { 
        return length_; 
    }
    
    int GetWidth() const { 
        return width_; 
    }
    
    int GetHeight() const { 
        return height_; 
    }
 
 private:
    int length_;
    int width_;
    int height_;
};
 
class Resources {
public:
    Resources(int bricks_count) : bricks_count_(bricks_count) {}
 
    void TakeBricks(int amount) {
        
        if ((bricks_count_-amount) < 0 || amount < 0) {//
            throw std::out_of_range("");
        }
        bricks_count_ -= amount;
    }
 
    int GetBrickCount() const { 
        return bricks_count_; 
    }
    
private:
    int bricks_count_;
};
 
struct HouseSpecification {
    int length = 0;
    int width = 0;
    int height = 0;
};
 
class Builder {
public:
    Builder(Resources& resources) : resources_(&resources) {}
 
    House BuildHouse(HouseSpecification specification) {
        
        if (2 * (specification.length + specification.width) * specification.height * 32 > resources_->GetBrickCount()) {
            throw std::runtime_error("");
        }
        
        resources_->TakeBricks(2 * (specification.length + specification.width) * specification.height * 32);
        return House(specification.length, specification.width, specification.height);
    }
private:
    Resources* resources_;
};
 
int main() {
    Resources resources{10000};
    Builder builder1{resources};
    Builder builder2{resources};
 
    House house1 = builder1.BuildHouse(HouseSpecification{12, 9, 3});
    assert(house1.GetLength() == 12);
    assert(house1.GetWidth() == 9);
    assert(house1.GetHeight() == 3);
    cout << resources.GetBrickCount() << " bricks left"s << endl;
 
    House house2 = builder2.BuildHouse(HouseSpecification{8, 6, 3});
    assert(house2.GetLength() == 8);
    cout << resources.GetBrickCount() << " bricks left"s << endl;
 
    House banya = builder1.BuildHouse(HouseSpecification{4, 3, 2});
    assert(banya.GetHeight() == 2);
    cout << resources.GetBrickCount() << " bricks left"s << endl;
}