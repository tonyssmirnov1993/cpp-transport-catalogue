#include <iostream>
#include <string>
#include <fstream>
#include "input_reader.h"
#include "stat_reader.h"
//#include "stat_reader.cpp"

//using namespace reader;
using namespace reader;
using namespace std;
using namespace stat_reader;

int main() {
    transport::TransportCatalogue catalogue;
    setlocale(LC_ALL, "rus");
    std::ifstream inp;
    inp.open("text.txt");
    if (inp.is_open()) {
        cout << "ФАЙЛ УСПЕШНО ОТКРЫТ...\n\n" << endl;
    }
    //исправлено
    {
        InputReader reader;

        reader.Input(inp,catalogue);     
    }
    std::ifstream req;
    req.open("req.txt");
   stat_reader:: Output(req, catalogue);
}