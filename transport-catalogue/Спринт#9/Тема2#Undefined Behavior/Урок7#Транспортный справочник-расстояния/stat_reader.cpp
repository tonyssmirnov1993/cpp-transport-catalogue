#include "stat_reader.h"

using namespace transport_catalogue::geo;

namespace transport_catalogue {

    namespace detail{

        void ParseAndPrintStat(TransportCatalogue& transport_catalogue, std::string_view request,
            std::ostream& output) {

            auto comm = request.substr(0, request.find(' '));
            auto descr = request.substr(comm.size() + 1, request.find_last_not_of(' '));

            if (comm == "Bus") {
                Bus* bus = transport_catalogue.GetBus(descr);

                
                if (bus != nullptr)
                {
                    auto dist = transport_catalogue.GetBusDistance(bus);
                    double curv_dist = 0.0;
                    int iter = 0;
                    while (iter != bus->bus_.size() - 1)
                    {
                        curv_dist += ComputeDistance({ bus->bus_[iter]->latitude_, bus->bus_[iter]->longitude_ },
                            { bus->bus_[iter + 1]->latitude_, bus->bus_[iter + 1]->longitude_ });
                        ++iter;
                    }
                    auto curvature =  static_cast<double>(dist)/ curv_dist;
                    auto size_uniq_stops = transport_catalogue.GetUniqueStops(bus).size();
                    output << "Bus " << bus->name_ << ": "
                        << bus->bus_.size() << " stops on route, "
                        << size_uniq_stops << " unique stops, "
                        << std::setprecision(6) << dist << " route length, " << curvature<< " curvature"<<std::endl;
                }
                else {
                    output << "Bus " << descr << ": " << "not found" << std::endl;
                }
            }
            else if (comm == "Stop") {
                std::unordered_set<const Bus*> unq_buses;
                std::vector<std::string_view> buses_at_stop;
                auto stop = transport_catalogue.GetStop(descr);

                if (stop != NULL) {
                    unq_buses = transport_catalogue.GetUniqueBuses(stop);
                    if (stop->buses_.size() == 0) {
                        output << "Stop" << " " << descr << ": " << "no buses" << std::endl;
                    }
                    else {
                        for (const Bus* bus : unq_buses) {
                            buses_at_stop.push_back(bus->name_);
                        }
                        std::sort(buses_at_stop.begin(), buses_at_stop.end());
                        output << "Stop" << " " << descr << ": buses ";
                        for (std::string_view bus : buses_at_stop) {
                            output << bus << " ";
                        }
                        output << std::endl;
                    }
                }
                else {
                    
                    output << "Stop" << " " << descr << ": " << "not found" << std::endl;
                    return;
                }
            }
        }

        void Output(TransportCatalogue& catalogue)
        {           
            int stat_request_count;
            std::cin >> stat_request_count >> std::ws;
            for (int i = 0; i < stat_request_count; ++i) {
                std::string line;
                getline(std::cin, line);
                ParseAndPrintStat(catalogue, line, std::cout);
            }
        }

        void OutputFromFile(TransportCatalogue& catalogue)
        {
            std::string file_name = "out.txt";
            std::string line;
            std::ifstream in(file_name);            
            if (in.is_open()) {                         
                while (std::getline(in, line))
                {
                    std::getline(in, line);
                    ParseAndPrintStat(catalogue, line, std::cout);                   
                }                           
            }
            else {
                std::cout << "incorrect file name" << std::endl;
                assert(false);
            }            
        }
    }
}