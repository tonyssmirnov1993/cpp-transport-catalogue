#pragma once
 
#include "people.h"
 
class SatisfactionSupervisor : public PersonObserver {
public:
    SatisfactionSupervisor(int min_satisfaction, int max_satisfaction) : min_(min_satisfaction), 
                                                                         max_(max_satisfaction) {}
 
    void OnSatisfactionChanged(Person& person, int old_value, int new_value) {
        
        if (new_value < min_ && (new_value - old_value) < 0) {
            
            while (person.GetSatisfaction() < max_) {
                person.Dance();
            }
        }
    }
    
private:
    int min_ = 0;
    int max_ = 0;
};