#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
   /* Реализуйте Node, используя std::variant */
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

    Node() = default;
    Node(Array array);
    Node(Dict map);
    Node(int value);
    Node(std::string value);
    Node(bool value);
    Node(double value);
    Node(std::nullptr_t);

    const Array& AsArray() const;
    const Dict& AsMap() const;
    int AsInt() const;
    double AsDouble()const;
    const std::string& AsString() const;
    bool AsBool() const;


    bool IsNull()const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsInt() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsMap() const;


    const Value& GetValue()const { return value_; };
private:
   /* Array as_array_;
    Dict as_map_;
    int as_int_ = 0;
    double as_double_ = 0.0;
    std::string as_string_;
    bool as_bool_ = false;
    std::nullptr_t as_null_;*/

     Value value_;
};

inline bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.GetValue() == rhs.GetValue();
}
inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);
inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}
inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

void Print(const Document& doc, std::ostream& output);
}  // namespace json