#include "stat_reader.h"

#include <iomanip>
#include <iostream>
#include <string>

namespace stat_reader {

    using namespace std;

    void ParseAndPrintStatBus(const transport::TransportCatalogue& transport_catalogue, std::string request,
        std::ostream& output) {
        output << "Bus "s << request << ": "s;
        if (transport_catalogue.GetBus(request)) {
            transport::StatRoute stat_route = transport_catalogue.GetStatBus(request);
            output << stat_route.number_of_stops << " stops on route, "s
                << stat_route.unique_stops << " unique stops, "s
                << std::setprecision(6) << stat_route.route_length << " route length\n"s;
        }
        else {
            output << "not found\n"s;
        }
    }

    void ParseAndPrintStatStop(const transport::TransportCatalogue& transport_catalogue, std::string request,
        std::ostream& output) {
        output << "Stop "s << request << ": "s;
        if (!transport_catalogue.GetStop(request)) {
            output << "not found\n"s;
            return;
        }
        const set<string_view>& buses = transport_catalogue.GetStatStop(request);
        if (buses.empty()) {
            output << "no buses\n"s;
            return;
        }
        output << "buses"s;
        for (const auto& bus : buses) {
            output << " "s << bus;
        }
        output << '\n';
    }

    void ParseAndPrintStat(const transport::TransportCatalogue& transport_catalogue, std::string_view request,
        std::ostream& output) {
        auto space_pos = request.find(' ');
        string key(string{ request.substr(0, space_pos) });
        string value(string{ request.substr(space_pos + 1) });
        if (key == "Bus") {
            ParseAndPrintStatBus(transport_catalogue, value, output);
        }
        else if (key == "Stop") {
            ParseAndPrintStatStop(transport_catalogue, value, output);
        }
    }
    //добавлено
    void Output(transport::TransportCatalogue& catalogue) {
        int stat_request_count;
        std::cin >> stat_request_count >> std::ws;
        for (int i = 0; i < stat_request_count; ++i) {
            std::string line;
            getline(std::cin, line);
            ParseAndPrintStat(catalogue, line, std::cout);
        }
    }
}//namespace stat    