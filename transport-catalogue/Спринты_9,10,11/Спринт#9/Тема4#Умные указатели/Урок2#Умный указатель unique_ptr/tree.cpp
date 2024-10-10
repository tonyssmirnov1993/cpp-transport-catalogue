#include <cassert>
#include <iostream>
#include <memory>
 
 
 
template <typename T>
struct TreeNode;
 
template <typename T>
using TreeNodePtr = std::unique_ptr<TreeNode<T>>;
 
template <typename T>
struct TreeNode {
    TreeNode(T val, TreeNodePtr<T>&& left, TreeNodePtr<T>&& right)
            : value(std::move(val))
            , left(std::move(left))
            , right(std::move(right)) {
    }
 
    T value;
    TreeNodePtr<T> left;
    TreeNodePtr<T> right;
    TreeNode* parent = nullptr;
};
 
template <class T>
bool CheckTreeProperty(const TreeNode<T>* node, const T* min, const T* max) noexcept {
    if (!node) {
        return true;
    }
    if (auto parent = node->parent) {
        if (parent->left.get() != node && parent->right.get() != node) {
            return false;
        }
    }
    if ((min && node->value <= *min) || (max && node->value >= *max)) {
        return false;
    }
    return CheckTreeProperty(node->left.get(), min, &node->value)
           && CheckTreeProperty(node->right.get(), &node->value, max);
}
 
template <class T>
bool CheckTreeProperty(const TreeNode<T>* node) noexcept {
    return CheckTreeProperty<T>(node, nullptr, nullptr);
}
 
template <class T>
TreeNode<T>* begin(TreeNode<T>* node) noexcept {
    while (node->left) {
        node = node->left.get();
    }
 
    return node;
}
 
template <class T>
TreeNode<T>* next(TreeNode<T>* node) noexcept {
    if (node->right) {
        return begin(node->right.get());
    }
    while (node->parent) {
        bool is_right = (node == node->parent->right.get());
        if (!is_right) {
            return node->parent;
        }
        node = node->parent;
    }
 
    return nullptr;
}
 
// функция создаёт новый узел с заданным значением и потомками
TreeNodePtr<int> N(int val, TreeNodePtr<int>&& left = {}, TreeNodePtr<int>&& right = {}) {
    auto node = std::make_unique<TreeNode<int>>(val, std::move(left), std::move(right));
    if (node->left) {
        node->left->parent = node.get();
    }
    if (node->right) {
        node->right->parent = node.get();
    }
 
    return node;
}
 
int main() {
    using namespace std;
    using T = TreeNode<int>;
    auto root1 = N(6, N(4, N(3), N(5)), N(7));
    assert(CheckTreeProperty(root1.get()));
 
    T* iter = begin(root1.get());
    while (iter) {
        cout << iter->value << " "s;
        iter = next(iter);
    }
    cout << endl;
 
    auto root2 = N(6, N(4, N(3), N(5)), N(7, N(8)));
    assert(!CheckTreeProperty(root2.get()));
 
    // Функция DeleteTree не нужна. Узлы дерева будут рекурсивно удалены
    // благодаря деструкторам unique_ptr
}