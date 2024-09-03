#include "people.h"
 
#include <stdexcept>
 
using namespace std;
 
void Programmer::AddProgrammingLanguage(ProgrammingLanguage language) {
    language_.insert(language);
}
 
bool Programmer::CanProgram(ProgrammingLanguage language) const {
    return language_.count(language);
}
 
void Worker::AddSpeciality(WorkerSpeciality speciality) {
    speciality_.insert(speciality);
}
 
bool Worker::HasSpeciality(WorkerSpeciality speciality) const {
    return speciality_.count(speciality);
}