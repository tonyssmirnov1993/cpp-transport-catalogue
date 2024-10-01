#pragma once

#include <iosfwd>
#include <iomanip>
#include <string_view>

#include "transport_catalogue.h"

namespace stat_reader {

    void ParseAndPrintStat(const transport::TransportCatalogue& transport_catalogue, std::string_view request,
        std::ostream& output);
    void Output(std::istream &inp, transport::TransportCatalogue& catalogue);
}//namespace stat