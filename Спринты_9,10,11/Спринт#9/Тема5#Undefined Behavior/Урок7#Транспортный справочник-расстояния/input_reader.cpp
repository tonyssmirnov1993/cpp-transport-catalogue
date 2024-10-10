#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <fstream>
using namespace transport_catalogue::geo;


namespace transport_catalogue {
    namespace detail {

        Coordinates ParseCoordinates(std::string_view str) {
            static const double nan = std::nan("");

            auto not_space = str.find_first_not_of(' ');
            auto comm = str.find(',');

            if (comm == str.npos) {
                return { nan, nan };
            }

            auto not_space_lng = str.find_first_not_of(' ', comm + 1);

            double lat = std::stod(std::string(str.substr(not_space, comm - not_space)));
            double lng = std::stod(std::string(str.substr(not_space_lng)));
            

            return { lat, lng };
        }

        std::string_view Trim(std::string_view string) {
            const auto start = string.find_first_not_of(' ');
            if (start == string.npos) {
                return {};
            }
            return string.substr(start, string.find_last_not_of(' ') + 1 - start);
        }

        std::vector<std::string_view> Split(std::string_view string, char delim) {
            std::vector<std::string_view> result;

            size_t pos = 0;
            while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
                auto delim_pos = string.find(delim, pos);
                if (delim_pos == string.npos) {
                    delim_pos = string.size();
                }
                if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                    result.push_back(substr);
                }
                pos = delim_pos + 1;
            }

            return result;
        }

        std::vector<std::string_view> ParseRoute(std::string_view route) {
            if (route.find('>') != route.npos) {
                return Split(route, '>');
            }

            auto stops = Split(route, '-');
            std::vector<std::string_view> results(stops.begin(), stops.end());
            results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

            return results;
        }

        CommandDescription ParseCommandDescription(std::string_view line) {
            auto colon_pos = line.find(':');
            if (colon_pos == line.npos) {
                return {};
            }

            auto space_pos = line.find(' ');
            if (space_pos >= colon_pos) {
                return {};
            }

            auto not_space = line.find_first_not_of(' ', space_pos);
            if (not_space >= colon_pos) {
                return {};
            }

            return { std::string(line.substr(0, space_pos)),
                    std::string(line.substr(not_space, colon_pos - not_space)),
                    std::string(line.substr(colon_pos + 1)) };
        }

        void InputReader::ParseLine(std::string_view line) {
            auto command_description = ParseCommandDescription(line);
            if (command_description) {
                commands_.push_back(std::move(command_description));
            }
        }

        std::string InputReader::TrimComma(std::string stop_name)const
        {
            auto temp_stop = Trim(stop_name);
            if (temp_stop.back() == ',') {
                temp_stop.remove_suffix(1);
                return std::string(temp_stop);
            }
            return std::string(temp_stop);
        }

        std::string InputReader::TrimCoord(std::string line)const
        {
            std::string result_line = "";
            int count_pos = 0;
            size_t pos = line.find("m to");
            for (size_t i = pos - 1; line[i] != ' '; --i)
            {
                ++count_pos;
            }
            result_line = line.substr(pos - count_pos);
            return std::string(result_line);
        }

        std::string InputReader::TrimDist(std::string line) const
        {
            std::string distance = "";
            size_t pos = line.find("m to");
            int count_pos = 0;
            
            for (size_t i = pos; i != 0 ; --i)
            {
                if (line[i] == ' ') { break; }
                ++count_pos;
            }
            distance = line.substr(pos - count_pos, count_pos);
            return distance;
        }
                
        void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
            for (size_t i = 0; i < commands_.size(); ++i)
            {
                if (commands_[i].command == "Stop") {

                    auto stop = catalogue.GetStop(commands_[i].id);

                    auto coord = ParseCoordinates(commands_[i].description);
                    
                    if (stop == nullptr) {
                        catalogue.AddStop(commands_[i].id, coord);
                        continue;
                    }
                    if (stop != nullptr && (stop->latitude_ + stop->longitude_ == 0.0)) {
                        stop->latitude_ = coord.lat;
                        stop->longitude_ = coord.lng;
                    }
                    else {
                        continue;
                    }
                }
                else if (commands_[i].command == "Bus") {
                    std::vector<std::string_view> stops_on_bus;
                    auto name_bus = commands_[i].id;
                    stops_on_bus = ParseRoute(commands_[i].description);
                    catalogue.AddRoute(name_bus, stops_on_bus);
                }
            }
        }

        void InputReader::InputFromFile([[maybe_unused]] TransportCatalogue& catalogue)
        {
            std::string base_request_count;
            std::string file_name = "tsC_case1_input.txt";
            
            std::ifstream in(file_name);
            if (in.is_open()) {
                std::getline(in, base_request_count);
                for (size_t i = 0; i < std::stoi(base_request_count); i++)
                {
                    std::string line;
                    std::getline(in, line);
                    ParseLine(line);
                }
                ApplyCommands(catalogue);
                ParseDistance(catalogue);
            }
            else {
                std::cout << "incorrect file name" << std::endl;
                assert(false);
            }
        }

        void InputReader::Input(TransportCatalogue& catalogue)
        {
            
            int base_request_count;
            std::cin >> base_request_count >> std::ws;
            for (int i = 0; i < base_request_count; ++i) {
                std::string line;
                getline(std::cin, line);
                ParseLine(line);
            }
            ApplyCommands(catalogue);
            ParseDistance(catalogue);
        }


        void InputReader::ParseDistance(TransportCatalogue& catalogue) const
        {
                        
            for (size_t i = 0; i < commands_.size(); ++i) {
                if (commands_[i].command == "Stop") {

                    std::vector<int> vec_distance;
                    std::vector<std::string> vec_stops;                   
                    size_t pos_to = 0;
                    auto line = commands_[i].description;
                   
                    if (line.find("to") == line.npos) {
                                             
                        continue;
                    }
                 
                    line = TrimCoord(line);

                    pos_to = line.find("m to");
                    
                    vec_distance.push_back(std::stoi(TrimDist(line)));
                    auto pos_stop = line.substr(pos_to + 4);
                    while (pos_stop.size() != 0)
                    {
                        pos_to = pos_stop.find("m to");                        
                        if (pos_to == pos_stop.npos) {
                            auto c_stop = TrimComma(pos_stop);
                            vec_stops.push_back(c_stop);
                            break;
                        }                      
                        vec_distance.push_back(std::stoi(TrimDist(pos_stop)));

                        auto pos_comma = pos_stop.find(',');
                        vec_stops.push_back(TrimComma(pos_stop.substr(0, pos_comma)));                       
                        pos_stop = pos_stop.substr(pos_to + 4);
                    }
                    
                    for (size_t i_v = 0; i_v < vec_stops.size(); i_v++)
                    {
                        catalogue.AddDistance(commands_[i].id, vec_stops[i_v], vec_distance[i_v]);
                    }
                }
            }        
        }
    }
}