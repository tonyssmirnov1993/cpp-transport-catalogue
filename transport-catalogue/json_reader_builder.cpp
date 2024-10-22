json::Builder result_builder;

    result_builder.StartArray();
    for (StatRequest req : stat_request) {
        if (req.type == "Stop") {

            std::vector<std::string> str_buses;
            std::string str_not_f = "not found";

            if (catalogue.GetStop(req.name) == nullptr) {
                result_builder.StartDict().Key("request_id").Value(req.id);
                result_builder.Key("error_message").Value(str_not_f).EndDict();


            }
            else {
                Stop* stop = catalogue.GetStop(req.name);
                result_builder.StartDict();
                result_builder.Key("request_id").Value(req.id);

                for (auto& bus : catalogue.GetUniqueBuses(stop)) {
                    str_buses.push_back(bus->name_);

                }
                std::sort(str_buses.begin(), str_buses.end());
                result_builder.Key("buses").StartArray();
                for (std::string& bus_s : str_buses) {
                    Bus* bus = catalogue.GetBus(bus_s);
                    result_builder.Value(bus->name_);
                }
                result_builder.EndArray().EndDict();

            }
        }
        else if (req.type == "Bus") {

            std::string str_not_f = "not found";
            Bus* bus = catalogue.GetBus(req.name);

            if (catalogue.GetBus(req.name) == nullptr) {
                result_builder.StartDict().Key("request_id").Value(req.id);
                result_builder.Key("error_message").Value(str_not_f).EndDict();


            }
            else {
                result_builder.StartDict().Key("request_id").Value(req.id);

                auto curv = double(catalogue.GetBusDistance(bus) / catalogue.GetLength(bus));


                result_builder.Key("curvature").Value(curv).Key("route_length").Value(catalogue.GetBusDistance(bus));
                result_builder.Key("stop_count").Value(int(bus->bus_.size())).Key("unique_stop_count").Value(int(catalogue.GetUniqueStops(bus).size()));
                result_builder.EndDict();
            }

        }
        if (req.type == "Map") {

            std::ostringstream map_stream;
            std::string line;

            rend.PrintMapOut(catalogue, map_stream);
            line = map_stream.str();

            result_builder.StartDict().Key("map").Value(line);
            result_builder.Key("request_id").Value(req.id).EndDict();


        }

    }
    result_builder.EndArray();
    return json::Document(json::Node{ result_builder.Build() });