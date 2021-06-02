//
// Created by jaoks on 5/17/20.
//

#ifndef B_TREE_V2_0_NODE_H
#define B_TREE_V2_0_NODE_H

#include <cmath>

namespace utec {
    namespace disk {
        template<class T, int BTREE_ORDER>
        struct Node {
            Node() {}

            long page_id{-1};
            long count{0};
            long right{0};

            T data[BTREE_ORDER + 1];
            long children[BTREE_ORDER + 2];


            Node(long page_id) : page_id{page_id} {
                count = 0;
                for (int i = 0; i < BTREE_ORDER + 2; i++) {
                    children[i] = 0;
                }
            }

            void insert_in_node(int pos, const T &value) {
                int j = count;
                while (j > pos) {
                    data[j] = data[j - 1];
                    children[j + 1] = children[j];
                    j--;
                }
                data[j] = value;
                children[j + 1] = children[j];

                count++;
            }

            void delete_in_node(int pos) {
                for (int i = pos; i < count; i++) {
                    data[i] = data[i + 1];
                    children[i + 1] = children[i + 2];
                }
                count--;
            }

            bool is_overflow() { return count > BTREE_ORDER; }

            bool is_underflow() { return count < floor(BTREE_ORDER / 2.0); }
        };
    }
}

#endif //B_TREE_V2_0_NODE_H
