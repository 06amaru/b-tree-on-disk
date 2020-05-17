//
// Created by jaoks on 5/17/20.
//

#ifndef B_TREE_V2_0_ITERATOR_H
#define B_TREE_V2_0_ITERATOR_H

#include "node.h"
#include "pagemanager.h"

namespace utec {
    namespace disk {
        template<class T, int B>
        class Iterator {
        public:
            typedef Node<T, B> node;
            int index = 0;
            node currentPosition;

            Iterator() {}

            Iterator(std::shared_ptr <pagemanager> pm) : pm{pm} {}

            T operator*() {
                return currentPosition.data[index];
            }

            Iterator &operator++() {
                if (index < currentPosition.count - 1) {
                    index++;
                } else {
                    index = 0;
                    node aux{-1};
                    if (currentPosition.right == 0) {  //consulta sentinela
                        currentPosition = aux;
                    } else {
                        this->pm->recover(currentPosition.right, currentPosition);
                    }
                }
                return *this;
            }

            Iterator &operator=(const Iterator &copy) {
                this->currentPosition = copy.currentPosition;
                this->index = copy.index;
                this->pm = copy.pm;
                return *this;
            }

            Iterator(const Iterator &copy) {
                this->currentPosition = copy.currentPosition;
                this->index = copy.index;
                this->pm = copy.pm;
            }

            bool operator!=(const Iterator &other) const {
                if (this->currentPosition.page_id == other.currentPosition.page_id) {
                    if (this->currentPosition.data[index] == other.currentPosition.data[other.index]) {
                        return false;
                    } else return true;
                }
                return true;
            }

        private:
            std::shared_ptr <pagemanager> pm;
        };
    }
}

#endif //B_TREE_V2_0_ITERATOR_H
