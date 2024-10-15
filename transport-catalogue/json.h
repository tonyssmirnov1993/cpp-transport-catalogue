#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

class Node;
using Dictionary = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
   /* Реализуйте Node, используя std::variant */
    using Value = std::variant<std::nullptr_t, Array, Dictionary, bool, int, double, std::string>;

    Node() = default;
    Node(Array array);
    Node(Dictionary map);
    Node(int value);
    Node(std::string value);
    Node(bool value);
    Node(double value);
    Node(std::nullptr_t);

    const Array& AsArray() const;
    const Dictionary& AsMap() const;
    const Value& GetValue()const { return value_; };
    const std::string& AsString() const;
   
    int AsInt() const;
    
    double AsDouble()const;

    bool AsBool() const;
    bool IsNull()const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsInt() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsMap() const;
    
private:  

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
    Document() = default;
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

}// namespace json