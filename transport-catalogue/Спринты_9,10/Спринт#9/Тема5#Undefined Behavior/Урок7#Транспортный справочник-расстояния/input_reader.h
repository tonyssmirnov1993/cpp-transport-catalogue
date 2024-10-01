#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace detail {
        struct CommandDescription {
            explicit operator bool() const {
                return !command.empty();
            }

            bool operator!() const {
                return !operator bool();
            }

            std::string command;      
            std::string id;           
            std::string description;  
        };

        class InputReader {
        public:                    
            void ParseLine(std::string_view line);   
            void ApplyCommands(TransportCatalogue& catalogue) const;
            void InputFromFile(TransportCatalogue& catalogue);
            void Input(TransportCatalogue& catalogue);
            void ParseDistance(TransportCatalogue& catalogue) const;
            std::string TrimComma(std::string stop_name)const;
            std::string TrimCoord(std::string line)const;
            std::string TrimDist(std::string line)const;

        private:
            std::vector<CommandDescription> commands_;
        };
    }
}