#include <functional>
#include <utility>
#include <cstdlib>

template<typename T> struct treap_node {
    T key;
    int priority;
    treap_node<T>* left;
    treap_node<T>* right;
    
    treap_node(const T& _key):
        key(_key),
        priority(rand() ^ (rand() << 15)),
        left(0),
        right(0)
    {}
};

template<typename T> struct treap {
    typedef treap_node<T>* node;
    typedef std::pair<node, node> node_pair;
    
    node root;
    int my_size;
    
    treap():
        root(0),
        my_size(0)
    {}
    virtual ~treap() {}
    
    bool insert(const T& x);
    bool erase(const T& x);
    bool contains(const T& x) const;
    int size() const;
    
    bool insert_slow(const T& x);
    bool erase_slow(const T& x);
    bool contains_slow(const T& x) const;
    
    node_pair treap_split(node v, const T& key, bool less_equal = true);
    node treap_merge(node left, node right);
};



template<typename T> typename treap<T>::node_pair treap<T>::treap_split(node v, const T& key, bool less_equal) {
    if (!v) return node_pair(0, 0);
    //TODO: rewrite to templates
    bool cmp_result = less_equal ? (!(key < v->key)) : (v->key < key);
    if (cmp_result) {
        node_pair p = treap_split(v->right, key, less_equal);
        v->right = p.first;
        p.first = v;
        return p;
    } else {
        node_pair p = treap_split(v->left, key, less_equal);
        v->left = p.second;
        p.second = v;
        return p;
    }
}

template<typename T> typename treap<T>::node treap<T>::treap_merge(node left, node right) {
    if (!left) return right;
    if (!right) return left;
    if (left->priority > right->priority) {
        left->right = treap_merge(left->right, right);
        return left;
    } else {
        right->left = treap_merge(left, right->left);
        return right;
    }
}


template<typename T> bool treap<T>::insert(const T& x) {
    return insert_slow(x);
}

template<typename T> bool treap<T>::erase(const T& x) {
    return erase_slow(x);
}

template<typename T> bool treap<T>::contains(const T& x) const {
    return contains_slow(x);
}

template<typename T> bool treap<T>::insert_slow(const T& x) {
    if (!root) {
        my_size++;
        root = new treap_node<T>(x);
        return true;
    }
    node_pair p = treap_split(root, x);
    node r = p.first;
    if (r) while (r->right)
        r = r->right;
    if (r && !(r->key < x) && !(x < r->key)) {
        root = treap_merge(p.first, p.second);
        return false;
    }
    my_size++;
    node v = new treap_node<T>(x);
    root = treap_merge(p.first, treap_merge(v, p.second));
    return true;
}

template<typename T> bool treap<T>::erase_slow(const T& x) {
    if (!root) return false;
    node_pair p = treap_split(root, x);
    if (!p.first) return false;
    node_pair q = treap_split(p.first, x, false);
    if (!q.second) return false;
    my_size--;
    root = treap_merge(q.first, p.second);
    return true;
}

template<typename T> bool treap<T>::contains_slow(const T& x) const {
    if (!root) return false;
    node_pair p = treap_split(root, x);
    if (!p.first) return false;
    node_pair q = treap_split(p.first, x, false);
    if (!q.second) return false;
    return true;
}

template<typename T> inline int treap<T>::size() const {
    return my_size;
}


#include <cassert>
#include <cstdio>
#include <ctime>
#include <set>

#define sz(v) ((int)((v).size()))
#define forn(i,n) for (int i = 0; i < (n); i++)

template<typename V> double measure(int n) {
    clock_t begin = clock();
    V v;
    forn(i, n) v.insert(i);
    assert(sz(v) == n);
    forn(i, n) v.insert(i);
    assert(sz(v) == n);
    forn(i, n) v.erase(i);
    assert(sz(v) == 0);
    forn(i, n) v.erase(i);
    assert(sz(v) == 0);
    clock_t end = clock();
    return (end - begin) * 1. / CLOCKS_PER_SEC;
}

int main() {
    const int n = 5000000;
    
    printf("  set: %.3lf\n", measure< std::set<int> >(n));
    printf("treap: %.3lf\n", measure< treap<int> >(n));
    
    return 0;
}
