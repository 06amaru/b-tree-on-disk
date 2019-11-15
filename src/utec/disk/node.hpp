namespace utec {

namespace disk {

template<class T, int BTREE_ORDER>
struct node {
    node() {}

    long page_id{-1};
    long count{0};
    long next{0};
    long prev{0};

    T data[BTREE_ORDER + 1];
    long children[BTREE_ORDER + 2];


    node(long page_id) : page_id{page_id} {
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

    bool is_overflow() { return count > BTREE_ORDER; }

};

}

}