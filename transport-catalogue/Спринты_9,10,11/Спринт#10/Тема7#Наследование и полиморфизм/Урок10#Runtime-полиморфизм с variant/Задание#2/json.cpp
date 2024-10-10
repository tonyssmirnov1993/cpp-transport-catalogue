#include "json.h"

using namespace std;

namespace json {

namespace {

    std::string LoadLiteral(std::istream& input) {
        std::string str;

        while (std::isalpha(input.peek())) {
            str.push_back(static_cast<char>(input.get()));
        }
        return str;
    }


Node LoadNode(istream& input);

Node LoadNull(istream& input) {

   auto s = LoadLiteral(input);
    
    if (s != "null") {
        throw ParsingError("String parsing error");
    }
    return Node();
}
Node LoadBool(istream& input) {
 
   auto s = LoadLiteral(input);
    if (s == "true") {
        return Node{ true };
    }
    else if(s == "false") {
        return Node{ false };
    }
    throw ParsingError("String parsing error");
}

Node LoadArray(istream& input) {
    Array result;
    //throw ParsingError("String parsing error");
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    if (it == end) {
        throw ParsingError("String parsing error");
    }
    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    return Node(move(result));
}

Node LoadNumber(std::istream& input) {
    using namespace std::literals;

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
                return Node{ std::stoi(parsed_num) };
            }
            catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return Node{ std::stod(parsed_num) };
    }
    catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

[[maybe_unused]] Node LoadInt(istream& input) {
    int result = 0;
    while (isdigit(input.peek())) {
        result *= 10;
        result += input.get() - '0';
    }
    return Node(result);
}

// Считывает содержимое строкового литерала JSON-документа
// Функцию следует использовать после считывания открывающего символа ":
Node LoadString(std::istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        }
        else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
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
                // Встретили неизвестную escape-последовательность
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        }
        else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        }
        else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return Node{ s };
}

Node LoadDict(istream& input) {
    Dict result;
    //throw ParsingError("String parsing error");
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    if (it == end) {
        // Поток закончился до того, как встретили закрывающую кавычку?
        throw ParsingError("String parsing error");
    }
    for (char c; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }

    return Node(move(result));
}

Node LoadNode(istream& input) {
    char ch;
    if (!(input >> ch)) {
        throw ParsingError("");
    }
    else {
        switch (ch)
        {
        case '[':
            return LoadArray(input);
        case '{':
            return LoadDict(input);
        case '"':
            return LoadString(input);
        case 'n':
            input.putback(ch);
            return LoadNull(input);
        case 't': case 'f':
            input.putback(ch);
            return LoadBool(input);
        default:
            input.putback(ch);
            return LoadNumber(input);
        }
    }
   
}

}  // namespace

Node::Node(Array array)
    : value_(std::move(array)) {
}

Node::Node(Dict map)
    : value_(std::move(map)) {
}

Node::Node(int value)
    : value_(value) {
}

Node::Node(string value)
    : value_(std::move(value)) {
}

Node::Node(bool value) : value_(value)
{
}

Node::Node(double value): value_(value)
{
}

Node::Node(std::nullptr_t) :Node ()
{
}


const Array& Node::AsArray() const {
    if (IsArray()) {
        return std::get<Array>(value_);
    }
    else {
        throw std::logic_error("");
    }
}

const Dict& Node::AsMap() const {
    if (IsMap()) {
        return std::get<Dict>(value_);
    }
    else {
        throw std::logic_error("");
    }
}

int Node::AsInt() const {
    if (IsInt()) {
        return std::get<int>(value_);
    }
    else {
        throw std::logic_error("");
    }
}

double Node::AsDouble() const
{
    if (!IsDouble()) {
        throw std::logic_error("");
    }
    else if (IsPureDouble()) {
        return std::get<double>(value_);
    }
    else {
        return AsInt();
    }
}

const string& Node::AsString() const {
    if (IsString()) {
        return std::get<string>(value_);
    }
    else {
        throw std::logic_error("");
    }
    
}

bool Node::AsBool() const
{
    if (IsBool()) {
        return std::get<bool>(value_);
    }
    else {
        throw std::logic_error("");
    }
   // return as_bool_;
}

bool Node::IsNull() const
{
   return std::holds_alternative<nullptr_t>(value_);
}

bool Node::IsDouble() const
{   
    if (std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_)) {
        return true;
    }
    return false;
}

bool Node::IsPureDouble() const
{
    return std::holds_alternative<double>(value_);
}

bool Node::IsInt() const
{
    return std::holds_alternative<int>(value_);
}

bool Node::IsBool() const
{
    return std::holds_alternative<bool>(value_);
}

bool Node::IsString() const
{
    return std::holds_alternative<std::string>(value_);
}

bool Node::IsArray() const
{
    return std::holds_alternative<Array>(value_);
}

bool Node::IsMap() const
{
    return std::holds_alternative<Dict>(value_);
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}



struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    [[nodiscard]] PrintContext Indented() const {
        return { out,
                indent_step,
                indent + indent_step };
    }
};

void PrintNode(const Node& node, const PrintContext& context);

void PrintString(const std::string& value, std::ostream& out) {
    out.put('"');

    for (const char ch : value) {
        switch (ch) {
        case '\r':
            out << R"(\r)";
            break;
        case '\n':
            out << R"(\n)";
            break;
        case '"':
            out << R"(\")";
            break;
        case '\\':
            out << R"(\\)";
            break;
        case '\t':
            out << R"(\t)";
            break;
        default:
            out.put(ch);
            break;
        }
    }

    out.put('"');
}

template <typename Value>
void PrintValue(const Value& value, const PrintContext& context) {
    context.out << value;
}

template <>
void PrintValue<std::string>(const std::string& value, const PrintContext& context) {
    PrintString(value, context.out);
}

void PrintValue(const std::nullptr_t&, const PrintContext& context) {
    context.out << "null"s;
}

void PrintValue(bool value, const PrintContext& context) {
    context.out << std::boolalpha << value;
}
//void PrintValue(string& val, PrintContext& contx) {
//    contx.out << val;
//}
[[maybe_unused]] void PrintValue(Array nodes, const PrintContext& context) {
    std::ostream& out = context.out;
    out << "[\n"sv;
    bool first = true;
    auto inner_context = context.Indented();

    for (const Node& node : nodes) {
        if (first) {
            first = false;
        }
        else {
            out << ",\n"sv;
        }

        inner_context.PrintIndent();
        PrintNode(node, inner_context);
    }

    out.put('\n');
    context.PrintIndent();
    out.put(']');
}

[[maybe_unused]] void PrintValue(Dict nodes, const PrintContext& context) {
    std::ostream& out = context.out;
    out << "{\n"sv;
    bool first = true;
    auto inner_context = context.Indented();

    for (const auto& [key, node] : nodes) {
        if (first) {
            first = false;
        }
        else {
            out << ",\n"sv;
        }

        inner_context.PrintIndent();
        PrintString(key, context.out);
        out << ": "sv;
        PrintNode(node, inner_context);
    }

    out.put('\n');
    context.PrintIndent();
    out.put('}');
}

void PrintNode(const Node& node, const PrintContext& context) {
    std::visit([&context](const auto& value) {
        PrintValue(value, context);
        }, node.GetValue());
}

void Print(const Document& document, std::ostream& output) {
    PrintNode(document.GetRoot(), PrintContext{ output });
}
}  // namespace json