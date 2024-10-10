#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "domain.h"
 
namespace transport_catalogue {
namespace detail {
namespace json {
    
using namespace transport_catalogue::detail::stop;
using namespace transport_catalogue::detail::bus;
    
struct StatRequest { 
    int id;
    std::string type;
    std::string name;    
};
    
void ParseNode(const Node& root, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request);
void Parse(Document& doc, TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request);
void ParseNodeBase(const Node& root, TransportCatalogue& catalogue);
void ParceNodeStat(const Node& root, std::vector<StatRequest>& stat_request);
 
Stop ParseNodeStop(Node& node);
std::vector<Distance> ParseNodeDistances(Node& node, TransportCatalogue& catalogue);
Bus ParseNodeBus(Node& node, TransportCatalogue& catalogue);
    
Document ExecuteQueries(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests);
Node ExecuteMakeNodeStop(int id_request, StopQueryResult query_result);
Node ExecuteMakeNodeBus(int id_request, BusQueryResult query_result);
    
}//end namespace json
}//end namespace detail
}//end namespace transport_catalogue