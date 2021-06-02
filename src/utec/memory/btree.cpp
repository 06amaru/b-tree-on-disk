#include <iostream>
#include <cmath>

namespace utec {

    namespace memory {


        template<class T, int BTREE_ORDER = 3>
        class btree {
            enum state {
                BT_OVERFLOW,
                BT_UNDERFLOW,
                NORMAL,
            };

            struct node {
                int count;
                T data[BTREE_ORDER + 1];
                node *children[BTREE_ORDER + 2];
                node *right = NULL;

                node() {
                    count = 0;
                    for (int i = 0; i < BTREE_ORDER + 2; i++) {
                        children[i] = NULL;
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

                bool is_underflow() {
                    return count < floor(BTREE_ORDER / 2.0);
                }
            };

            node *root;

        public:
            btree() { root = new node; }

            void insert(const T &value) {
                int state = insert(root, value);
                if (state == BT_OVERFLOW) {
                    split_root(root, value);
                }
            }

            int insert(node *ptr, const T &value) {
                int pos = 0;
                while (pos < ptr->count && ptr->data[pos] < value) {
                    pos++;
                }
                if (ptr->children[pos] != NULL) {
                    int state = insert(ptr->children[pos], value);
                    if (state == BT_OVERFLOW) {
                        split(ptr, pos);
                    }
                } else {
                    ptr->insert_in_node(pos, value);
                }
                return ptr->is_overflow() == true ? BT_OVERFLOW : NORMAL;
            }

            void split(node *ptr, int pos) {
                node *node_in_overflow = ptr->children[pos];
                node *child1 = node_in_overflow;
                child1->count = 0;
                node *child2 = new node();

                int iter = 0;
                int i;
                for (i = 0; iter < ceil(BTREE_ORDER / 2.0); i++) {
                    child1->children[i] = node_in_overflow->children[iter];
                    child1->data[i] = node_in_overflow->data[iter];
                    child1->count++;

                    iter++;
                }
                child1->children[i] = node_in_overflow->children[iter];

                ptr->insert_in_node(pos, node_in_overflow->data[iter]);

                if (node_in_overflow->children[0] != NULL) {
                    iter++; // the middle element
                } else {
                    child2->right = child1->right;
                    child1->right = child2;
                    ptr->children[pos + 1] = child2;
                }

                for (i = 0; iter < BTREE_ORDER + 1; i++) {
                    child2->children[i] = node_in_overflow->children[iter];
                    child2->data[i] = node_in_overflow->data[iter];
                    child2->count++;
                    iter++;
                }
                child2->children[i] = node_in_overflow->children[iter];

                ptr->children[pos] = child1;
                ptr->children[pos + 1] = child2;
            }

            void split_root(node *ptr, const T &value) {
                node *node_in_overflow = ptr;
                node *child1 = new node();
                node *child2 = new node();

                int iter = 0;
                int i;

                for (i = 0; i < ceil(BTREE_ORDER / 2.0); i++) {
                    child1->children[i] = node_in_overflow->children[iter];
                    child1->data[i] = node_in_overflow->data[iter];
                    child1->count++;

                    iter++;
                }
                child1->children[i] = node_in_overflow->children[iter];

                ptr->data[0] = node_in_overflow->data[iter];
                child1->right = child2;
                if (ptr->children[0] != NULL) ++iter;
                for (i = 0; iter < BTREE_ORDER + 1; i++) {
                    child2->children[i] = node_in_overflow->children[iter];
                    child2->data[i] = node_in_overflow->data[iter];
                    child2->count++;

                    iter++;
                }
                child2->children[i] = node_in_overflow->children[iter];

                ptr->children[0] = child1;
                ptr->children[1] = child2;
                ptr->count = 1;
            }

            void remove(const T &value) {
                int state = remove(root, value);
                if (state == BT_UNDERFLOW && root->count == 0) {
                    root = root->children[0];
                }
            }

            int remove(node *ptr, const T &value) {
                int pos = 0;
                while (pos < ptr->count && ptr->data[pos] < value) {
                    pos++;
                }
                if (ptr->children[pos] != NULL) { //si el nodo no es hoja
                    //encontrar el siguiente sucesor y suplantarlo
                    if (ptr->data[pos] == value && pos != ptr->count) {
                        ptr->data[pos] = succesor(ptr->children[pos + 1]);
                        pos++;
                    }
                    int state = remove(ptr->children[pos], value);
                    if (state == BT_UNDERFLOW) {
                        node *node_in_underflow = ptr->children[pos];
                        //ver si algun hermano de izq o der puede pasar 1 nodo
                        bool can_steal = steal_sibling(node_in_underflow, ptr, pos);
                        if (!can_steal) { //si no se puede robar entonces
                            if (ptr->children[pos]->children[0] == NULL) { // si el hijo es hoja
                                merge_leaf(ptr, node_in_underflow, pos);
                            } else {
                                bool can_merge = merge_with_parent(ptr, node_in_underflow, pos);
                                if (!can_merge) decrease_height(ptr, node_in_underflow, pos);
                            }
                        }
                    }
                } else if (ptr->data[pos] == value) {
                    ptr->delete_in_node(pos);
                }

                return ptr->is_underflow() == true ? BT_UNDERFLOW : NORMAL;
            }

            void decrease_height(node *ptr, node *node_in_underflow, int pos) {
                if (pos != ptr->count) {
                    if (ptr->children[pos]->count < floor(BTREE_ORDER / 2.0)) { //underflow desde la izquierda
                        //insertar dato del padre al hermano
                        node *sibling = ptr->children[pos + 1];
                        sibling->insert_in_node(0, ptr->data[pos]);
                        // 1 hijo del hno ahora es el ultimo hijo del underflow
                        int last = node_in_underflow->count;
                        sibling->children[0] = node_in_underflow->children[last];
                        for (int i = last - 1; i >= 0; --i) {
                            sibling->insert_in_node(0, node_in_underflow->data[i]);
                            sibling->children[0] = node_in_underflow->children[i];
                        }
                        //elimino el dato que bajo
                        ptr->delete_in_node(pos);
                        //el 1 hijo del padre ahora es el hno
                        ptr->children[pos] = sibling;
                        return;
                    }
                }
                node *sibling = ptr->children[pos - 1];
                int last = sibling->count;
                sibling->insert_in_node(last, ptr->data[pos - 1]); //insert en la ult pos data padre
                sibling->children[last + 1] = node_in_underflow->children[0]; //el ptr al child en underflow
                for (int i = 0; i < node_in_underflow->count; i++) {
                    last = sibling->count;
                    sibling->insert_in_node(last, node_in_underflow->data[i]);
                    sibling->children[last + 1] = node_in_underflow->children[i + 1];
                }
                //elimino el dato que bajo
                ptr->delete_in_node(pos - 1);
                //el 1 hijo del padre ahora es el hno
                ptr->children[pos - 1] = sibling;

            }

            bool merge_with_parent(node *ptr, node *node_in_underflow, int pos) {
                //checkear si se puede robar a la izq o a la der
                if (pos != 0) {
                    //el caso de robar al hijo de la izquierda
                    if ((ptr->children[pos - 1]->count - 1) >= floor(BTREE_ORDER / 2.0)) { //puede prestar 1 dato?
                        node *sibling = ptr->children[pos - 1];
                        T jesus = ptr->data[pos - 1]; //dato del padre
                        //el padre baja donde el nodo en underflow
                        node_in_underflow->insert_in_node(0, jesus);
                        //el hermano sube al lugar donde estaba el padre
                        ptr->data[pos - 1] = sibling->data[sibling->count - 1];
                        //pasamos el ult hijo del hermano para que ahora sea el 1er hijo del node_in_underflow
                        node_in_underflow->children[0] = sibling->children[sibling->count];
                        //eliminar el hermano que subio
                        sibling->delete_in_node(sibling->count - 1);
                        return true;
                    }
                } else if (pos != BTREE_ORDER) {
                    //el caso de robar al hijo de la derecha
                    if ((ptr->children[pos + 1]->count - 1) >= floor(BTREE_ORDER / 2.0)) {
                        node *sibling = ptr->children[pos + 1];
                        T jesus = ptr->data[pos]; //dato del padre
                        //el padre baja donde el nodo en underflow
                        node_in_underflow->insert_in_node(0, jesus);
                        //el hermano sube al lugar donde estaba el padre
                        ptr->data[pos] = sibling->data[0];
                        //pasamos el 1er hijo del hermano para que ahora sea el 2do hijo del node_in_underflow
                        node_in_underflow->children[1] = sibling->children[0];
                        sibling->children[0] = sibling->children[1];
                        //eliminar el hermano que subio
                        sibling->delete_in_node(0);
                        return true;
                    }
                }
                return false;
            }


            void merge_leaf(node *ptr, node *node_in_underflow, int pos) {
                if (pos - 1 >= 0) {  //derecha se une a izquierda
                    node *sibling = ptr->children[pos - 1];
                    for (int i = 0; i < node_in_underflow->count; i++) {
                        int pos_in = sibling->count;
                        sibling->insert_in_node(pos_in, node_in_underflow->data[i]);
                    }
                    sibling->right = node_in_underflow->right; //ignoramos al hermano
                    ptr->delete_in_node(pos - 1);
                } else {  //izquierda se une a derecha
                    node *sibling = ptr->children[1];
                    for (int i = 0; i < sibling->count; i++) {
                        int pos_in = node_in_underflow->count;
                        node_in_underflow->insert_in_node(pos_in, sibling->data[i]);
                    }
                    node_in_underflow->right = sibling->right; //ignoramos al hermano
                    ptr->delete_in_node(0);
                }
            }

            bool steal_sibling(node *node_in_underflow, node *ptr, int pos) {
                if (ptr->children[0]->children[0] == NULL) { //solo puedes robar en nodos hojas
                    if (pos != ptr->count) {
                        if (ptr->children[pos + 1]->count - 1 >= floor(BTREE_ORDER / 2.0)) {
                            node *sibling = ptr->children[pos + 1]; //hermano de la derecha
                            T jesus = sibling->data[0];
                            T jesus2 = sibling->data[1];
                            sibling->delete_in_node(0);
                            node_in_underflow->insert_in_node(sibling->count - 1, jesus);
                            ptr->data[pos] = jesus2;
                            return true;
                        }
                    }
                    if (pos > 0) {
                        if (ptr->children[pos - 1]->count - 1 >= floor(BTREE_ORDER / 2.0)) {
                            node *sibling = ptr->children[pos - 1];  //hermano de la izquierda
                            T jesus = sibling->data[sibling->count - 1];  //el valor mas a la derecha
                            sibling->delete_in_node(sibling->count - 1);
                            node_in_underflow->insert_in_node(0, jesus);
                            ptr->data[pos - 1] = jesus;
                            return true;
                        }
                    }
                }
                return false;
            }

            T succesor(node *ptr) {
                while (ptr->children[0] != NULL) {
                    ptr = ptr->children[0];
                }
                if (ptr->count == 1) {
                    if (ptr->right == NULL) return -1; //en el caso de encontrar el succesor del mayor valor
                    return ptr->right->data[0];
                } else {
                    return ptr->data[1];
                }
            }

            void print() {
                print(root, 0);
                std::cout << "________________________\n";
            }

            void print(node *ptr, int level) {
                if (ptr) {
                    int i;
                    for (i = ptr->count - 1; i >= 0; i--) {
                        print(ptr->children[i + 1], level + 1);

                        for (int k = 0; k < level; k++) {
                            std::cout << "    ";
                        }
                        std::cout << ptr->data[i] << "\n";
                    }
                    print(ptr->children[i + 1], level + 1);
                }
            }

            void inorder() {
                inorder(root, 0);
            }

            void inorder(node *ptr, int level) {
                if (ptr) {
                    int i;
                    for (i = 0; i < ptr->count; i++) {
                        inorder(ptr->children[i], level + 1);
                        std::cout << ptr->data[i] << ", ";
                    }
                    inorder(ptr->children[i], level + 1);
                }
            }
        };
    } // namespace memory
} // namespace utec