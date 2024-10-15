#include <iostream>
#include "json_reader.h"
#include "map_renderer.h"

using namespace svg;
using namespace std;
using namespace transport_catalogue;
using namespace map_renderer;

int main() {
    {
        vector<StatRequest> stat_request;
        TransportCatalogue catalogue;
        Renderer renderer;
        JsonReader reader;

        {

            std::ifstream input("input.json");

            json::Document document_input;
            json::Document document_output;

            std::ofstream output_test("output.svg");

            document_input = json::Load(input);

            reader.LoadJson(document_input, catalogue, stat_request, renderer);

            document_output = reader.ExecuteQuries(catalogue, stat_request, renderer);

            json::Print(document_output, std::cout);
        }
    }
}