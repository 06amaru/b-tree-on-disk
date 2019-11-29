#pragma once

#include "pagemanager.hpp"
#include "node.hpp"
#include "iterator.hpp"
#include <memory>

namespace utec {
namespace disk {

template <class T, int BTREE_ORDER = 3> class btree {
public:
  
  typedef node<T,BTREE_ORDER> node;
  typedef Iterator<T,BTREE_ORDER> iterator;

  iterator begin() {
    node aux = read_node(header.root_id);
    while(aux.children[0] != 0) {
      long page_id = aux.children[0];
      aux = read_node(page_id);  
    }
    iterator it(pm);
    it.currentPosition = aux;
    return it;
  }

  iterator find(const T &object){
    node root = read_node(header.root_id);
    auto it = find(object, root);
    if ( *it == object ) {
      return it;
    } else {
      return end();
    }
  }

  iterator find(const T &object , const node &other) {
    int pos = 0;
    if(other.children[0] != 0) {
      while(pos < other.count && other.data[pos] <= object) {
        pos++;
      }
      node child = read_node(other.children[pos]);
      return find(object, child);
    } else {
      while(pos < other.count && other.data[pos] < object) {
        pos++;
      }
      iterator it(pm);
      it.currentPosition = other;
      it.index = pos;
      return it;
    }
  }

  iterator end() {
    node aux{-1};
    iterator it(pm);
    it.currentPosition = aux;
    return it;
  }

  struct Metadata {
    long root_id{1};
    long count{0};
  } header;

  enum state {
    BT_OVERFLOW,
    BT_UNDERFLOW,
    NORMAL,
  };

private:
  std::shared_ptr<pagemanager> pm;

public:
  btree(std::shared_ptr<pagemanager> pm) : pm{pm} {
    if (pm->is_empty()) {
      node root{header.root_id};
      pm->save(root.page_id, root);

      header.count++;

      pm->save(0, header);
    } else {
      pm->recover(0, header);
    }
  }

  node new_node() {
    header.count++;
    node ret{header.count};
    pm->save(0, header);
    return ret;
  }

  node read_node(long page_id) {
    node n{-1};
    pm->recover(page_id, n);
    return n;
  }

  void write_node(long page_id, node n) { pm->save(page_id, n); }

  void insert(const T &value) {
    node root = read_node(header.root_id);
    int state = insert(root, value);

    if (state == BT_OVERFLOW) {
      split_root();
    }
  }

  int insert(node &ptr, const T &value) {
    int pos = 0;
    while (pos < ptr.count && ptr.data[pos] < value) {
      pos++;
    }
    if (ptr.children[pos] != 0) {
      long page_id = ptr.children[pos];
      node child = read_node(page_id);
      int state = insert(child, value);
      if (state == BT_OVERFLOW) {
        split(ptr, pos);
      }
    } else {
      ptr.insert_in_node(pos, value);
      write_node(ptr.page_id, ptr);
    }
    return ptr.is_overflow() ? BT_OVERFLOW : NORMAL;
  }

  void split(node &parent, int pos) {
    node node_in_overflow = this->read_node(parent.children[pos]);
    node child1 = node_in_overflow;
    child1.count = 0;
    node child2 = this->new_node();

    int iter = 0;
    int i;
    for (i = 0; iter < ceil(BTREE_ORDER / 2.0); i++) {
      child1.children[i] = node_in_overflow.children[iter];
      child1.data[i] = node_in_overflow.data[iter];
      child1.count++;
      iter++;
    }
    child1.children[i] = node_in_overflow.children[iter];

    parent.insert_in_node(pos, node_in_overflow.data[iter]);

    if(node_in_overflow.children[0] != 0){
      iter++;
    } else {
      child2.right = child1.right;
      child1.right = child2.page_id;
      parent.children[pos+1] = child2.page_id;
    }

    for (i = 0; iter < BTREE_ORDER + 1; i++) {
      child2.children[i] = node_in_overflow.children[iter];
      child2.data[i] = node_in_overflow.data[iter];
      child2.count++;
      iter++;
    }
    child2.children[i] = node_in_overflow.children[iter];

    parent.children[pos] = child1.page_id;
    parent.children[pos + 1] = child2.page_id;

    write_node(parent.page_id, parent);
    write_node(child1.page_id, child1);
    write_node(child2.page_id, child2);
  }

  void split_root() {
    node node_in_overflow = this->read_node(this->header.root_id);
    node child1 = this->new_node();
    node child2 = this->new_node();

    int pos = 0;
    int iter = 0;
    int i;
    for (i = 0; iter < ceil(BTREE_ORDER / 2.0); i++) {
      child1.children[i] = node_in_overflow.children[iter];
      child1.data[i] = node_in_overflow.data[iter];
      child1.count++;
      iter++;
    }
    child1.children[i] = node_in_overflow.children[iter];

    node_in_overflow.data[0] = node_in_overflow.data[iter];

    child1.right = child2.page_id;

    if(node_in_overflow.children[0] != 0){
      iter++; // the middle element
    }

    for (i = 0; iter < BTREE_ORDER + 1; i++) {
      child2.children[i] = node_in_overflow.children[iter];
      child2.data[i] = node_in_overflow.data[iter];
      child2.count++;
      iter++;
    }
    child2.children[i] = node_in_overflow.children[iter];

    node_in_overflow.children[0] = child1.page_id;
    node_in_overflow.children[1] = child2.page_id;
    node_in_overflow.count = 1;

    write_node(node_in_overflow.page_id, node_in_overflow);
    write_node(child1.page_id, child1);
    write_node(child2.page_id, child2);
  }

  void remove(const T &value) {
      node root = read_node(header.root_id);
      int state = remove(root, value);
      ///root = read_node(header.root_id);
      if(state == BT_UNDERFLOW && root.count == 0) {
          header.root_id = root.children[0];
          write_node(root.page_id, root);
          pm->save(0,header);
      }
  }

  int remove(node &ptr, const T &value) {
    int pos = 0;
    while (pos < ptr.count && ptr.data[pos] < value) {
        pos++;
    }
    if(ptr.children[pos] != 0){
        if(ptr.data[pos] == value && pos != ptr.count) {
            node next = read_node(ptr.children[pos+1]);
            ptr.data[pos] = succesor(next);
            write_node(ptr.page_id, ptr);
            pos++;
        }
        node child = read_node(ptr.children[pos]);
        int state = remove(child, value);
        if (state == BT_UNDERFLOW) {
            node node_in_underflow = child;
            bool can_steal = steal_sibling(node_in_underflow, ptr, pos);
            if (!can_steal) {
                if(node_in_underflow.children[0] == 0) { //si el underflow es hoja
                    merge_leaf(ptr, node_in_underflow, pos);
                } else {
                    bool can_merge = merge_with_parent(ptr, node_in_underflow, pos);
                    if(!can_merge) decrease_height(ptr, node_in_underflow, pos);
                }
            }
        }
    } else if (ptr.data[pos] == value) {
        ptr.delete_in_node(pos);
        write_node(ptr.page_id, ptr);
    }

    return ptr.is_underflow() ? BT_UNDERFLOW : NORMAL;
  }

  void decrease_height( node &ptr, node &node_in_underflow, int pos) {
      if (pos != ptr.count) {
          node child = read_node(ptr.children[pos]);
          if(child.count < floor(BTREE_ORDER/2.0)) {
              node sibling = read_node(ptr.children[pos+1]);
              sibling.insert_in_node(0, ptr.data[pos]);
              int last = node_in_underflow.count;
              sibling.children[0] = node_in_underflow.children[last];

              for(int i = last - 1; i >= 0; --i) {
                  sibling.insert_in_node(0,node_in_underflow.data[i]);
                  sibling.children[0] = node_in_underflow.children[i];
              }
              ptr.delete_in_node(pos);
              ptr.children[pos] = sibling.page_id;
              write_node(sibling.page_id, sibling);
              write_node(ptr.page_id, ptr);
              return;
          }
      }
      node sibling = read_node(ptr.children[pos - 1]);
      int last = sibling.count;
      sibling.insert_in_node(last, ptr.data[pos-1]);
      sibling.children[last+1]=node_in_underflow.children[0];
      for(int i = 0; i < node_in_underflow.count; i++) {
          last = sibling.count;
          sibling.insert_in_node(last, node_in_underflow.data[i]);
          sibling.children[last+1] = node_in_underflow.children[i+1];
      }
      ptr.delete_in_node(pos-1);
      ptr.children[pos-1]=sibling.page_id;

      write_node(sibling.page_id, sibling);
      write_node(ptr.page_id, ptr);
  }

  bool merge_with_parent(node &ptr, node &node_in_underflow, int pos) {
      if(pos != 0) {
          node sibling = read_node(ptr.children[pos - 1 ]);
          if ( sibling.count - 1 >= floor(BTREE_ORDER/2.0)) {
              T jesus = ptr.data[pos - 1];
              node_in_underflow.insert_in_node(0, jesus);
              ptr.data[pos-1] = sibling.data[sibling.count-1];
              node_in_underflow.children[0] = sibling.children[sibling.count];
              sibling.delete_in_node(sibling.count-1);
              write_node(sibling.page_id, sibling);
              write_node(ptr.page_id, ptr);
              write_node(node_in_underflow.page_id, node_in_underflow);
              return true;
          }
      } else if (pos != BTREE_ORDER) {
        node sibling = read_node(ptr.children[pos + 1]);
        if ( sibling.count - 1 >= floor(BTREE_ORDER/2.0)) {
            T jesus = ptr.data[pos];
            node_in_underflow.insert_in_node(0, jesus);
            ptr.data[pos] = sibling.data[0];
            node_in_underflow.children[1] = sibling.children[0];
            sibling.children[0] = sibling.children[1];
            sibling.delete_in_node(0);

            write_node(sibling.page_id, sibling);
            write_node(ptr.page_id, ptr);
            write_node(node_in_underflow.page_id, node_in_underflow);
            return true;
        }
      }
      return false;
  }

  void merge_leaf(node &ptr, node &node_in_underflow, int pos) {
      if (pos  -  1 >=  0 ) { //derecha se une a izquierda
          node sibling = read_node(ptr.children[pos-1]);
          for(int i = 0; i  < node_in_underflow.count; i++) {
              int pos_in = sibling.count;
              sibling.insert_in_node(pos_in, node_in_underflow.data[i]);
          }
          sibling.right = node_in_underflow.right;
          ptr.delete_in_node(pos - 1);

          write_node(sibling.page_id, sibling);
          write_node(ptr.page_id, ptr);

      } else { //izquierda se une a la derecha
          node sibling = read_node(ptr.children[1]);
          for(int i = 0; i < sibling.count; i++) {
              int pos_in = node_in_underflow.count;
              node_in_underflow.insert_in_node(pos_in, sibling.data[i]);
          }
          node_in_underflow.right = sibling.right;
          ptr.delete_in_node(0);
          write_node(node_in_underflow.page_id, node_in_underflow);
          write_node(ptr.page_id, ptr);
      }
  }

  bool steal_sibling(node& node_in_underflow, node& ptr, int pos) {
      if (node_in_underflow.children[0] == 0) { //verificar que es hoja
          if (pos != ptr.count) {
              node sibling = read_node(ptr.children[pos+1]); //hno de la derecha
              if (sibling.count - 1 >= floor(BTREE_ORDER/2.0)) {
                  T jesus = sibling.data[0];
                  T jesus2 = sibling.data[1];
                  sibling.delete_in_node(0);
                  node_in_underflow.insert_in_node(sibling.count-1, jesus);
                  ptr.data[pos] = jesus2;
                  write_node(sibling.page_id, sibling);
                  write_node(node_in_underflow.page_id, node_in_underflow);
                  write_node(ptr.page_id, ptr);
                  return true;
              }
          }
          if (pos > 0) {
              node sibling = read_node(ptr.children[pos-1]); //hno de la izquierda
              if (sibling.count - 1 >= floor(BTREE_ORDER/2.0)) {
                  T jesus = sibling.data[sibling.count - 1 ];
                  sibling.delete_in_node(sibling.count -1);
                  node_in_underflow.insert_in_node(0, jesus);
                  ptr.data[pos - 1 ] = jesus;
                  write_node(sibling.page_id, sibling);
                  write_node(node_in_underflow.page_id, node_in_underflow);
                  write_node(ptr.page_id, ptr);
                  return true;
              }
          }
      } return false;
  }

  T succesor(node &ptr) {
      while(ptr.children[0] != 0) {
          ptr = read_node(ptr.children[0]);
      }
      if (ptr.count == 1){
          if(ptr.right == -1) return NULL;
          ptr  = read_node(ptr.right);
          return ptr.data[0];
      } else {
          return ptr.data[1];
      }
  }


    void print(std::ostream& out) {
        node root = read_node(header.root_id);
        print(root, 0, out);
    }

    void print(node &ptr, int level, std::ostream& out) {
        int i;
        for (i = 0; i < ptr.count; i++) {
            if (ptr.children[i]) {
                node child = read_node(ptr.children[i]);
                print(child, level + 1, out);
            }
            out << ptr.data[i];
        }
        if (ptr.children[i]) {
            node child = read_node(ptr.children[i]);
            print(child, level + 1, out);
        }
    }


    void print_tree() {
        node root = read_node(header.root_id);
        print_tree(root, 0);
        std::cout << "________________________\n";
    }

    void print_tree(node &ptr, int level) {
        int i;
        for (i = ptr.count - 1; i >= 0; i--) {
            if (ptr.children[i + 1]) {
                node child = read_node(ptr.children[i + 1]);
                print_tree(child, level + 1);
            }

            for (int k = 0; k < level; k++) {
                std::cout << "    ";
            }
            std::cout << ptr.data[i] << "\n";
        }
        if (ptr.children[i + 1]) {
            node child = read_node(ptr.children[i + 1]);
            print_tree(child, level + 1);
        }
    }
};


} // namespace disk
} // namespace utec