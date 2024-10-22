#include "json.h"

#include <iterator>

namespace json {

    namespace {
        using namespace std::literals;

        Node LoadNode(std::istream& input);
        Node LoadString(std::istream& input);

        std::string LoadLiteral(std::istream& input) {
            std::string str;
            while (std::isalpha(input.peek())) {
                str.push_back(static_cast<char>(input.get()));
            }
            return str;
        }

        Node LoadArray(std::istream& input) {
            std::vector<Node> result;

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (!input) {
                throw ParsingError("Array parsing error"s);
            }
            return Node(std::move(result));
        }

        Node LoadDictionary(std::istream& input) {
            Dictionary dictionary;

            for (char c; input >> c && c != '}';) {
                if (c == '"') {
                    std::string key = LoadString(input).AsString();
                    if (input >> c && c == ':') {
                        if (dictionary.find(key) != dictionary.end()) {
                            throw ParsingError("Duplicate key '"s + key + "' have been found"s);
                        }
                        dictionary.emplace(std::move(key), LoadNode(input));
                    }
                    else {
                        throw ParsingError(": is expected but '"s + c + "' has been found"s);
                    }
                }
                else if (c != ',') {
                    throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
                }
            }
            if (!input) {
                throw ParsingError("Dictionary parsing error"s);
            }
            return Node(std::move(dictionary));
        }

        Node LoadString(std::istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(std::move(s));
        }

        Node LoadBool(std::istream& input) {
            const auto s = LoadLiteral(input);
            if (s == "true"sv) {
                return Node{ true };
            }
            else if (s == "false"sv) {
                return Node{ false };
            }
            else {
                throw ParsingError("Failed to parse '"s + s + "' as bool"s);
            }
        }

        Node LoadNull(std::istream& input) {
            if (auto literal = LoadLiteral(input); literal == "null"sv) {
                return Node{ nullptr };
            }
            else {
                throw ParsingError("Failed to parse '"s + literal + "' as null"s);
            }
        }

        Node LoadNumber(std::istream& input) {
            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
                };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
                };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadNode(std::istream& input) {
            char c;
            if (!(input >> c)) {
                throw ParsingError("Unexpected EOF"s);
            }
            switch (c) {
            case '[':
                return LoadArray(input);
            case '{':
                return LoadDictionary(input);
            case '"':
                return LoadString(input);
            case 't':
                // Атрибут [[fallthrough]] (провалиться) ничего не делает, и является
                // подсказкой компилятору и человеку, что здесь программист явно задумывал
                // разрешить переход к инструкции следующей ветки case, а не случайно забыл
                // написать break, return или throw.
                // В данном случае, встретив t или f, переходим к попытке парсинга
                // литералов true либо false
                [[fallthrough]];
            case 'f':
                input.putback(c);
                return LoadBool(input);
            case 'n':
                input.putback(c);
                return LoadNull(input);
            default:
                input.putback(c);
                return LoadNumber(input);
            }
        }

        struct PrintContext {
            std::ostream& output;
            int indent_step = 4;
            int indent = 0;

            void PrintIndent() const {
                for (int i = 0; i < indent; ++i) {
                    output.put(' ');
                }
            }

            PrintContext Indented() const {
                return { output, indent_step, indent_step + indent };
            }
        };

        void PrintNode(const Node& value, const PrintContext& ctext);

        template <typename Value>
        void PrintValue(const Value& value, const PrintContext& ctext) {
            ctext.output << value;
        }

        void PrintString(const std::string& value, std::ostream& output) {
            output.put('"');
            for (const char c : value) {
                switch (c) {
                case '\r':
                    output << "\\r"sv;
                    break;
                case '\n':
                    output << "\\n"sv;
                    break;
                case '\t':
                    output << "\\t"sv;
                    break;
                case '"':
                    // Символы " и \ выводятся как \" или \\, соответственно
                    [[fallthrough]];
                case '\\':
                    output.put('\\');
                    [[fallthrough]];
                default:
                    output.put(c);
                    break;
                }
            }
            output.put('"');
        }

        template <>
        void PrintValue<std::string>(const std::string& value, const PrintContext& ctext) {
            PrintString(value, ctext.output);
        }

        template <>
        void PrintValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& ctext) {
            ctext.output << "null"sv;
        }

        // В специализации шаблона PrintValue для типа bool параметр value передаётся
        // по константной ссылке, как и в основном шаблоне.
        // В качестве альтернативы можно использовать перегрузку:
        // void PrintValue(bool value, const PrintContext& ctx);
        template <>
        void PrintValue<bool>(const bool& value, const PrintContext& ctext) {
            ctext.output << (value ? "true"sv : "false"sv);
        }

        template <>
        void PrintValue<Array>(const Array& nodes, const PrintContext& ctext) {
            std::ostream& output = ctext.output;
            output << "[\n"sv;
            bool first = true;
            auto inner_ctext = ctext.Indented();
            for (const Node& node : nodes) {
                if (first) {
                    first = false;
                }
                else {
                    output << ",\n"sv;
                }
                inner_ctext.PrintIndent();
                PrintNode(node, inner_ctext);
            }
            output.put('\n');
            ctext.PrintIndent();
            output.put(']');
        }

        template <>
        void PrintValue<Dictionary>(const Dictionary& nodes, const PrintContext& ctext) {
            std::ostream& output = ctext.output;
            output << "{\n"sv;
            bool first = true;
            auto inner_ctext = ctext.Indented();
            for (const auto& [key, node] : nodes) {
                if (first) {
                    first = false;
                }
                else {
                    output << ",\n"sv;
                }
                inner_ctext.PrintIndent();
                PrintString(key, ctext.output);
                output << ": "sv;
                PrintNode(node, inner_ctext);
            }
            output.put('\n');
            ctext.PrintIndent();
            output.put('}');
        }

        void PrintNode(const Node& node, const PrintContext& ctext) {
            std::visit(
                [&ctext](const auto& value) {
                    PrintValue(value, ctext);
                },
                node.GetValue());
        }

    }  // namespace

    Document Load(std::istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& document, std::ostream& output) {
        PrintNode(document.GetRoot(), PrintContext{ output });
    }

}  // namespace json json