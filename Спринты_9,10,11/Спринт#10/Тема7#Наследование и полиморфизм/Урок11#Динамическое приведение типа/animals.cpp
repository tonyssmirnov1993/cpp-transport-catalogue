#include <iostream>
#include <string>
#include <string_view>
#include <vector>
 
using namespace std;
 
class Speakable {
public:
    virtual ~Speakable() = default;
    virtual void Speak(ostream& out) const = 0;
};
 
class Drawable {
public:
    virtual ~Drawable() = default;
    virtual void Draw(ostream& out) const = 0;
};
 
class Animal {
public:
    virtual ~Animal() = default;
    void Eat(string_view food) {
        cout << GetType() << " is eating "sv << food << endl;
        ++energy_;
    }
    virtual string GetType() const = 0;
 
private:
    int energy_ = 100;
};
 
class Fish : public Animal, public Drawable {
public:
    string GetType() const override {
        return "fish"s;
    }
    void Draw(ostream& out) const override {
        out << "><(((*>"sv << endl;
    }
};
 
class Cat : public Animal, public Speakable, public Drawable {
public:
    void Speak(ostream& out) const override {
        out << "Meow-meow"sv << endl;
    }
    void Draw(ostream& out) const override {
        out << "(^w^)"sv << endl;
    }
    string GetType() const override {
        return "cat"s;
    }
};
 
void DrawAnimals(const std::vector<const Animal*>& animals, ostream& out) {
    for(auto animal_ : animals) {        
        if(const Drawable* animal = dynamic_cast<const Drawable*>(animal_)){
            animal->Draw(out);
        }
    }
}
 
void TalkToAnimals(const std::vector<const Animal*> animals, ostream& out) {
    for(auto animal_ : animals) {        
        if(const Speakable* animal = dynamic_cast<const Speakable*>(animal_)){
            animal->Speak(out);
        }
    }
}
 
void PlayWithAnimals(const std::vector<const Animal*> animals, ostream& out) {
    TalkToAnimals(animals, out);
    DrawAnimals(animals, out);
}
 
int main() {
    Cat cat;
    Fish fish;
    vector<const Animal*> animals{&cat, &fish};
    PlayWithAnimals(animals, cerr);
}