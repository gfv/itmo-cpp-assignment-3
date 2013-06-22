#include <functional>
#include <iostream>
#include <random>
using namespace std;

template <typename T>
class Node {
public:
    Node* left;
    Node* right;
    int parents;
    T value;

    Node(const Node<T> * another) {
        if (another == nullptr) { left = nullptr; right = nullptr; value = 0; parents = 1; return; }
        left = another->left;
        right = another->right;
        value = another->value;
        parents = 1;
    }

    Node(Node<T>* left, Node<T>* right, T value) : left(left), right(right), value(value), parents(1) { }


    Node(T value) {
        left = nullptr;
        right = nullptr;
        this->value = value;
        parents = 1;
    }
};

template <typename T>
class Tree {
    Node<T>* root;
    
    Node<T>* findLeast(Node<T>* root) {
        if (root->left == nullptr) { return root; }
        return findLeast(root->left);
    }

    Node<T>* rNode(Node<T>* cur, T value) {
        if (value < cur->value) {
            //bumpNode(cur->right);
            return new Node<T>(rNode(cur->left, value), cur->right, cur->value);
        } else if (value > cur->value) {
            //bumpNode(cur->left);
            return new Node<T>(cur->left, rNode(cur->right, value), cur->value);
        }
        
        if (cur->left == nullptr && cur->right == nullptr) {
            GC(cur);
            return nullptr;
        }

        if (cur->left != nullptr && cur->right == nullptr) {
            bumpNode(cur->left);
            auto nc = cur->left;
            GC(cur);
            return nc;
        }

        if (cur->left == nullptr && cur->right != nullptr) {
            bumpNode(cur->right);
            auto nc = cur->right;
            GC(cur);
            return nc;
        }

        T v = findLeast(cur->right)->value;
        bumpNode(cur->left);
        bumpNode(cur->right);
        auto n = new Node<T>(cur->left, rNode(cur->right, v), v);
        GC(cur);
        return n;
    }

    Node<T>* aNode(Node<T>* root, T value) {
        if (root == nullptr) return new Node<T>(value);
        if (value < root->value) {
            bumpNode(root->right);
            return new Node<T>(aNode(root->left, value), root->right, root->value);
        }
        if (value > root->value) {
            bumpNode(root->left);
            return new Node<T>(root->left, aNode(root->right, value), root->value);
        }
    }


    Node<T>* locateNode(T value, Node<T>* root) const {
        if (root == nullptr) return nullptr;
        if (value < root->value) return locateNode(value, root->left);
        if (value > root->value) return locateNode(value, root->right);
        return root;
    }

    void bumpNode(Node<T>* node) {
        if (node != nullptr) node->parents++;
    }

    void GC(Node<T>* node) {
        if (node == nullptr) return;
        node->parents--;
        if (node->parents <= 0) {
            GC(node->left);
            GC(node->right);
            delete node;
        }
    }

public:
    Tree() {
        root = nullptr;
    }

    Tree(Node<T> * node) {
        if (node == nullptr) root = nullptr;
        else { root = node; bumpNode(root); }
    }

    Tree(const Tree<T> &another) {
        if (another.getRoot() == nullptr) root = nullptr;
        else { root = another.getRoot(); bumpNode(root); }
    }

    ~Tree() {
        GC(root);
    }

    Node<T>* getRoot() const {
        return root;
    }

    Tree add(T value) {
        if (exists(value)) return Tree(root);
        return Tree(aNode(root, value));
    }

    Tree remove(T value) {
        if (!exists(value)) return Tree(root);
        return Tree(rNode(root, value));
    }

    bool exists(T value) {
        return locateNode(value, root) != nullptr;
    }

    void traverseTree(std::function<void(Node<T>*)> f) {
        internalTraverseTree(f, root);
    }

    void internalTraverseTree(std::function<void(Node<T>*)> f, Node<T>* root) {
        if (root == nullptr) { f(root); return; }
        internalTraverseTree(f, root->left);
        f(root);
        internalTraverseTree(f, root->right);
    }
};

int main (int c, char** a) {
    Tree<int> t;
    Tree<int> n = t.add(4).add(2).add(1).add(3).add(6).add(5).add(7);
    auto trf = [](Node<int>* ptr){ if (ptr == nullptr) return; cerr<< ptr->value << "P" << ptr->parents<<endl; };
    n.traverseTree(trf);

    auto q = n.remove(6);
    
    n.traverseTree(trf);
    cerr<<endl<<endl;
    q.traverseTree(trf);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 10000);
 
    vector<Tree<int>> v;
    v.push_back(Tree<int>());
    for (int i = 0; i < 10000; i++) {
        int a = dis(gen);
        if (a>=5000) v.push_back(v.back().add(a-5000));
        else v.push_back(v.back().remove(a));
    }
            
    return 0;
}
