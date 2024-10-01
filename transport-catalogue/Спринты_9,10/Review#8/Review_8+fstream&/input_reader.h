#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

namespace reader {

    struct CommandDescription {
        // ����������, ������ �� ������� (���� command ��������)
        explicit operator bool() const {
            return !command.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string command;      // �������� �������
        std::string id;           // id �������� ��� ���������
        std::string description;  // ��������� �������
    };

    class InputReader {
    public:
        void ParseLine(std::string_view line);
        void ApplyCommands(transport::TransportCatalogue& catalogue) const;
        void Input(std::istream& inp, transport::TransportCatalogue& catalogue);
    private:
        std::vector<CommandDescription> commands_;
    };
}//namespace reader