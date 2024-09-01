#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace reader;
using namespace std;
using namespace stat_reader;

int main() {
    transport::TransportCatalogue catalogue;
    //исправлено
    {
        InputReader reader;
        reader.Input(catalogue);   
    }
    Output(catalogue);
}