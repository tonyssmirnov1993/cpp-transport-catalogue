#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;
using namespace transport_catalogue;
using namespace transport_catalogue::detail;

int main() {
    TransportCatalogue catalogue;          
    {
        InputReader reader;
        reader.Input(catalogue);              
    }
    Output(catalogue); 
}