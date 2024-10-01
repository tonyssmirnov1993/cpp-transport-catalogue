#pragma once
 
#include <string>
 
class Person;
 
class PersonObserver {
public:
    virtual void OnSatisfactionChanged(Person&, int, int) {}
 
protected:
    ~PersonObserver() = default;
};
 
class Person {
public:
    Person(const std::string& name, int age) : name_(name),
                                               age_(age) {}
    virtual ~Person() = default;
 
    const std::string& GetName() const {
        return name_;
    }
    
    int GetAge() const {
        return age_;
    }
    
    int GetSatisfaction() const {
        return satisfaction_;
    }
    
    auto GetObserver() const {
        return observer_;
    }
 
    void SetObserver(PersonObserver* observer) {
        observer_ = observer;
    }
 
    virtual void Dance() {
        ++dance_count_;
        SetSatisfaction(satisfaction_ + 1);
    }
 
    int GetDanceCount() const {
        return dance_count_;
    }
 
    virtual void LiveADay() {}
 
protected:
    
    virtual void SetSatisfaction(int value) {
        if (satisfaction_ != value) {
            int old_value = satisfaction_;
            satisfaction_ = value;
            if (observer_) {
                observer_->OnSatisfactionChanged(*this, old_value, satisfaction_);
            }
        }
    }
 
private:
    std::string name_;
    int age_;
    int satisfaction_ = 100;
    PersonObserver* observer_ = nullptr;
    int dance_count_ = 0;
};
 
class Worker : public Person {
public:
    Worker(const std::string& name, int age) : Person(name, age) {}
 
    void Dance() override {
        Person::Dance();
        
        if (int age = GetAge(); age > 30 && age < 40) {
            SetSatisfaction(GetSatisfaction() + 1);
        }
    }
 
    void Work() {
        ++work_;
        SetSatisfaction(GetSatisfaction() - 5);
    }
 
    int GetWorkDone() const {
        return work_;
    }
 
    void LiveADay() override {
        Work();
    }
    
private:
    int work_ = 0;
};
 
class Student : public Person {
public:
    Student(const std::string& name, int age) : Person(name, age) {}
 
    void Study() {
        ++study_;
        SetSatisfaction(GetSatisfaction() - 3);
    }
 
    void Dance() override {
        auto old_value = GetSatisfaction();
        Person::Dance();
        
        if (GetObserver()) {
            GetObserver()->OnSatisfactionChanged(*this, old_value, GetSatisfaction());
        }
    }
 
    int GetKnowledgeLevel() const {
        return study_;
    }
 
    void LiveADay() override {
        Study();
    }
    
private:
    int study_ = 0;
};