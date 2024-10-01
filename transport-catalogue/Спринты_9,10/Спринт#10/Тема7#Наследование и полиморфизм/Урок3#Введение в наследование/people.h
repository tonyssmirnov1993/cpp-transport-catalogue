#pragma once
#include <string>
#include <set>
 
enum class ProgrammingLanguage { CPP, JAVA, PYTHON, PHP };
 
enum class Gender { MALE, FEMALE };
 
class Person {
public:
    Person(const std::string& name, int age, Gender gender) : name_(name), 
                                                              age_(age), 
                                                              gender_(gender) {}
    const std::string& GetName() const {
        return name_;
    }
 
    int GetAge() const {
        return age_;
    }
 
    Gender GetGender() const {
        return gender_;
    }
    
private:
    std::string name_;
    int age_;
    Gender gender_;
};
 
class Programmer : public Person {
public:
    Programmer(const std::string& name, int age, Gender gender) : Person(name, 
                                                                         age, 
                                                                         gender) {}
 
    void AddProgrammingLanguage(ProgrammingLanguage language);
    bool CanProgram(ProgrammingLanguage language) const;
    
private:
    std::set<ProgrammingLanguage> language_;
};
 
enum class WorkerSpeciality { BLACKSMITH, CARPENTER, WOOD_CHOPPER, ENGINEER, PLUMBER };
 
class Worker : public Person {
public:
    Worker(const std::string& name, int age, Gender gender) : Person(name, 
                                                                     age, 
                                                                     gender) {}
 
    void AddSpeciality(WorkerSpeciality speciality);
    bool HasSpeciality(WorkerSpeciality speciality) const;
    
private:
    std::set<WorkerSpeciality> speciality_;
};