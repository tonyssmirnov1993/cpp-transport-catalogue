#include "transport_catalogue.h"
#include "json_reader.h"
#include "domain.h"
 
using namespace std;
using namespace transport_catalogue;
using namespace transport_catalogue::detail;
using namespace transport_catalogue::detail::json;
 
int main() {
    std::vector<StatRequest> stat_request;
    
    TransportCatalogue catalogue;
    
    Document doc_in;
    Document doc_out;
    
    doc_in = json::Load(std::cin);
    
    json::Parse(doc_in, catalogue, stat_request);
    doc_out = json::ExecuteQueries(catalogue, stat_request);
    Print(doc_out, std::cout);
}