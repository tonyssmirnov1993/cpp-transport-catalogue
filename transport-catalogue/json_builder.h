#pragma once
#include "json.h"
#include <optional>

namespace json {

    class Builder {
    public:
        class DictionaryItemContext;
        class DictionaryKeyContext;
        class ArrayItemContext;

        Builder();
        DictionaryKeyContext Key(std::string key);
        Builder& Value(Node::Value value);
        DictionaryItemContext StartDictionary();
        Builder& EndDictionary();
        ArrayItemContext StartArray();
        Builder& EndArray();
        Node Build();
        Node GetNode(Node::Value value);

    private:
        Node root_{ nullptr };
        std::vector<Node*> nodes_stack_;
        std::optional<std::string> key_{ std::nullopt };
    };

    class Builder::DictionaryItemContext {
    public:
        DictionaryItemContext(Builder& builder);

        DictionaryKeyContext Key(std::string key);
        Builder& EndDictionary();

    private:
        Builder& builder_;
    };

    class Builder::ArrayItemContext {
    public:
        ArrayItemContext(Builder& builder);

        ArrayItemContext Value(Node::Value value);
        DictionaryItemContext StartDict();
        Builder& EndArray();
        ArrayItemContext StartArray();

    private:
        Builder& builder_;
    };

    class Builder::DictionaryKeyContext {
    public:
        DictionaryKeyContext(Builder& builder);

        DictionaryItemContext Value(Node::Value value);
        ArrayItemContext StartArray();
        DictionaryItemContext StartDict();

    private:
        Builder& builder_;
    };

} // namespace json