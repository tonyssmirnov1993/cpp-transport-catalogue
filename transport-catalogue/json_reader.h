#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

using namespace transport_catalogue;

struct StatRequest {
    int id;
    std::string type;
    std::string name;
};


class JsonReader {
public:
    json::Document ExecuteQueries(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_request, map_renderer::Renderer& render);
    void LoadJson(json::Document& document, TransportCatalogue& tq, std::vector<StatRequest>& stat_request, map_renderer::Renderer& render);
    
   
private:
    json::Dictionary UpdExecuteStop(StatRequest& req, TransportCatalogue& catalogue);
    json::Dictionary UpdExecuteBuses(StatRequest& req, TransportCatalogue& catalogue);
    json::Dictionary UpdExecuteMap(StatRequest& req, TransportCatalogue& catalogue, map_renderer::Renderer& render);
    void ParseNode(const json::Node& root, TransportCatalogue& tq, std::vector<StatRequest>& stat_request, map_renderer::Renderer& render);
    void ParseNodeStat(const json::Node& node, std::vector<StatRequest>& stat_request);
    void ParseBaseRequests(const json::Node& document, TransportCatalogue& tq);
    //====================Renderer======================
    void ParseRenderRequests(const json::Node& document, /*TransportCatalogue& tq,*/ map_renderer::Renderer& render);
    void ParseUnderColor(const json::Node& node,/*map_renderer::Renderer& rend*/map_renderer::RenderSettings& settings);
    void ParseColorPalette(const json::Node& node,/* map_renderer::Renderer& rend*/map_renderer::RenderSettings& settings);
    void ParseBus(const json::Node& node, TransportCatalogue& tq);
    void ParseStop(const json::Node& node, TransportCatalogue& tq);
    void ParseDistance(TransportCatalogue& tq);

   json::Dictionary stops_distances_;

public:
    
    JsonReader() = default;
};