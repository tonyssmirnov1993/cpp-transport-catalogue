#pragma once

#include <iosfwd>
#include <iomanip>
#include <string_view>

#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace detail {
        void ParseAndPrintStat(TransportCatalogue& tansport_catalogue, std::string_view request,
            std::ostream& output);
        void Output(TransportCatalogue& catalogue);
    }
}