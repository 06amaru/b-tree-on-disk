//
// Created by jaoks on 5/16/20.
//

#include "gtest/gtest.h"
#include "../../src/utec/memory/btree.cpp"

struct MemoryBasedBtree : public ::testing::Test {
};


TEST_F(MemoryBasedBtree, TestB){
using namespace utec::memory;

    btree<int, 5> bt;
    bt.insert(42);
    bt.insert(19);
    bt.insert(85);
    bt.insert(27);
    //bt.print();

    bt.insert(63);
    bt.insert(35);
    bt.insert(18);
    //bt.print();

    bt.insert(99);
    //bt.print();

    bt.insert(87);
    //bt.print();

    bt.insert(21);
    //bt.print();

    bt.insert(76);
    //bt.print();

    bt.insert(54);
    std::cout << "LAST INSERT"<< std::endl;
    bt.print();

    bt.remove(85);
    bt.print();

    bt.remove(87);
    bt.print();

    bt.remove(63);
    bt.print();

    bt.remove(19);
    bt.print();

    bt.remove(21);
    bt.print();

    bt.remove(35);
    bt.print();

    bt.remove(54);
    bt.print();

    std::cout<< "MERGE LEFT PARENT" << std::endl;
    bt.remove(76);  //si quieres ver un merge with parent caso izquierda
    bt.print();

    bt.remove(42);  //si quieres ver un merge leaf
    bt.print();

    bt.remove(99);
    bt.print();

    bt.remove(18);
    bt.print();

}

TEST_F(MemoryBasedBtree, TestC) {
using namespace utec::memory;
    btree<char> bt;
    std::string abc = "qruvefghicdlmnojkwxyzabpst";
    for (int i = 0;i<abc.size();++i) {
        bt.insert(abc[i]);
        bt.print();
    }

    for (int i = abc.size() - 1;i >= 0; --i) {
        std::cout << "DELETE "<<abc[i]<<'\n';
        bt.remove(abc[i]);
        bt.print();
    }
}


//TEST_F(MemoryBasedBtree, TestF) {
//    using namespace utec::memory;
//    btree<int> bt;
//    int input[15] = {6,2,3,7,1,5,12,9,8,0,15,21,19,27,13};
//
//    for (int i = 0; i < 15; ++i) {
//        bt.insert(input[i]);
//    }
//    bt.print();
//    std::cout << " ###### IN ORDER #########\n";
//    bt.inorder();
//}
//
//TEST_F(MemoryBasedBtree, TestG) {
//    using namespace utec::memory;
//    btree<int> bt;
//    int input[5] = {1, 1, 2, 3, 5};
//
//    for (int i = 0; i < 5; ++i) {
//        bt.insert(input[i]);
//    }
//    bt.print();
//    std::cout << " ###### IN ORDER #########\n";
//    bt.inorder();
//}