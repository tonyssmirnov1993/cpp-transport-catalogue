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
        reader.Input(catalogue);    //‘class reader::InputReader’ has no member named ‘Input’          
    }
    Output(catalogue);   //‘Output’ was not declared in this scope
}
//exposed/submission/input_reader.h:26:7: note: ‘class reader::InputReader’ defined here class InputReader {