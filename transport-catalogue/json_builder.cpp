#include "json_builder.h"
#include <variant>

using namespace std::literals;

namespace json {

    Builder::Builder()
        : root_()
        , nodes_stack_{ &root_ }
    {}

    Node Builder::Build() {
        if (!nodes_stack_.empty()) {
            throw std::logic_error("Attempt to build JSON which isn't finalised"s);
        }
        return std::move(root_);
    }

    Builder::DictionaryValueContext Builder::Key(std::string key) {
        Node::Value& host_value = GetCurrentValue();

        if (!std::holds_alternative<Dictionary>(host_value)) {
            throw std::logic_error("Key() outside a dictionary"s);
        }

        nodes_stack_.push_back(
            &std::get<Dictionary>(host_value)[std::move(key)]
        );
        return BaseContext{ *this };
    }

    Builder::BaseContext Builder::Value(Node::Value value) {
        AddObject(std::move(value), /* one_shot */ true);
        return *this;
    }

    Builder::DictionaryItemContext Builder::StartDictionary() {
        AddObject(Dictionary{}, /* one_shot */ false);
        return BaseContext{ *this };
    }

    Builder::ArrayItemContext Builder::StartArray() {
        AddObject(Array{}, /* one_shot */ false);
        return BaseContext{ *this };
    }

    Builder::BaseContext Builder::EndDictionary() {
        if (!std::holds_alternative<Dictionary>(GetCurrentValue())) {
            throw std::logic_error("EndDictionary() outside a dictionary"s);
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Builder::BaseContext Builder::EndArray() {
        if (!std::holds_alternative<Array>(GetCurrentValue())) {
            throw std::logic_error("EndDictionary() outside an array"s);
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Node::Value& Builder::GetCurrentValue() {
        if (nodes_stack_.empty()) {
            throw std::logic_error("Attempt to change finalised JSON"s);
        }
        return nodes_stack_.back()->GetValue();
    }

    // Tell about this trick
    const Node::Value& Builder::GetCurrentValue() const {
        return const_cast<Builder*>(this)->GetCurrentValue();
    }

    void Builder::AssertNewObjectContext() const {
        if (!std::holds_alternative<std::nullptr_t>(GetCurrentValue())) {
            throw std::logic_error("New object in wrong context"s);
        }
    }

    void Builder::AddObject(Node::Value value, bool one_shot) {
        Node::Value& host_value = GetCurrentValue();
        if (std::holds_alternative<Array>(host_value)) {
            // Tell about emplace_back
            Node& node
                = std::get<Array>(host_value).emplace_back(std::move(value));
            if (!one_shot) {
                nodes_stack_.push_back(&node);
            }
        }
        else {
            AssertNewObjectContext();
            host_value = std::move(value);
            if (one_shot) {
                nodes_stack_.pop_back();
            }
        }
    }
}  