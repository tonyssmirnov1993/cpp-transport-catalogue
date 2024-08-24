#include <iostream>
#include <algorithm>
 
using namespace std::literals;
 
const std::string_view PLANETS[] = {
    "Mercury"sv, 
    "Venus"sv, 
    "Earth"sv,
    "Mars"sv, 
    "Jupiter"sv, 
    "Saturn"sv,
    "Uranus"sv, 
    "Neptune"sv,
};
 
bool IsPlanet(std::string_view name) {        
    return std::count(begin(PLANETS), end(PLANETS), name) ? true : false;
}
 
void Test(std::string_view name) {
    std::cout << name << " is " 
              << (IsPlanet(name) ? ""sv : "NOT "sv)
              << "a planet"sv << std::endl;
}
 
int main() {
    std::string planet;
    getline(std::cin, planet);
    Test(planet);
}