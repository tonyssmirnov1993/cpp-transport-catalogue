#pragma once
#include <string>
#include <vector>
#include "json.h"

namespace json {

class Builder {
private:
    class BaseContext;
    class DictionaryValueContext;
    class DictionaryItemContext;
    class ArrayItemContext;

public:
    Builder();
    Node Build();
    DictionaryValueContext Key(std::string key);
    BaseContext Value(Node::Value value);
    DictionaryItemContext StartDictionary();
    ArrayItemContext StartArray();
    BaseContext EndDictionary();
    BaseContext EndArray();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;

    Node::Value& GetCurrentValue();
    const Node::Value& GetCurrentValue() const;

    void AssertNewObjectContext() const;
    void AddObject(Node::Value value, bool one_shot);

    class BaseContext {
    public:
        BaseContext(Builder& builder) : builder_(builder) {}
        Node Build() {
            return builder_.Build();
        }
        DictionaryValueContext Key(std::string key) {
            return builder_.Key(std::move(key));
        }
        BaseContext Value(Node::Value value) {
            return builder_.Value(std::move(value));
        }
        DictionaryItemContext StartDictionary() {
            return builder_.StartDictionary();
        }
        ArrayItemContext StartArray() {
            return builder_.StartArray();
        }
        BaseContext EndDictionary() {
            return builder_.EndDictionary();
        }
        BaseContext EndArray() {
            return builder_.EndArray();
        }
    private:
        Builder& builder_;
    };

    class DictionaryValueContext : public BaseContext {
    public:
        DictionaryValueContext(BaseContext base) : BaseContext(base) {}
        DictionaryItemContext Value(Node::Value value) { return BaseContext::Value(std::move(value)); }
        Node Build() = delete;
        DictionaryValueContext Key(std::string key) = delete;
        BaseContext EndDict() = delete;
        BaseContext EndArray() = delete;
    };

    class DictionaryItemContext : public BaseContext {
    public:
        DictionaryItemContext(BaseContext base) : BaseContext(base) {}
        Node Build() = delete;
        BaseContext Value(Node::Value value) = delete;
        BaseContext EndArray() = delete;
        DictionaryItemContext StartDictionary() = delete;
        ArrayItemContext StartArray() = delete;
    };

    class ArrayItemContext : public BaseContext {
    public:
        ArrayItemContext(BaseContext base) : BaseContext(base) {}
        ArrayItemContext Value(Node::Value value) { return BaseContext::Value(std::move(value)); }
        Node Build() = delete;
        DictionaryValueContext Key(std::string key) = delete;
        BaseContext EndDictionary() = delete;
    };
};
} //namespace json