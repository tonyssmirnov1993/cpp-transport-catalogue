#pragma once
 
#include "json.h"
#include <stack>
#include <string>
#include <memory>
 
namespace json {
    
class Builder {
public:
    
    Node make_node(Node::Value value_);
    void add_node(Node node);
    
    Builder& Key(std::string key_);
    Builder& Value(Node::Value value_);
    
    Builder& StartDict();
    Builder& EndDict();
    
    Builder& StartArray();
    Builder& EndArray();
    
    Node Build();
    
private:
    Node root_; 
    std::vector<std::unique_ptr<Node>>nodes_stack_; 
    
};
    
}//end namespace json