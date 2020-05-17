//
// Created by jaoks on 5/16/20.
//

#include "gtest/gtest.h"
#include "../../src/utec/memory/btree.cpp"

struct MemoryBasedBtree : public ::testing::Test {
};


TEST_F(MemoryBasedBtree, TestA
) {
using namespace utec::memory;

btree<int, 4> bt;
bt.insert(10);
bt.insert(5);
bt.insert(8);
bt.insert(9);
bt.

print();

bt.insert(17);
bt.

print();

bt.insert(21);
bt.

print();

bt.insert(25);
bt.

print();

bt.insert(44);
bt.

print();

bt.insert(48);
bt.

print();

bt.insert(59);
bt.

print();

bt.remove(21);
bt.

print();

bt.remove(5);
bt.

print();

bt.remove(10);
bt.

print();

bt.remove(8);
bt.

print();

bt.insert(49);
bt.

print();

bt.insert(60);
bt.

print();

bt.remove(17);
bt.

print();

bt.remove(59);
bt.

print();

bt.remove(60);
bt.

print();

bt.remove(48);
bt.

print();

bt.remove(25);
bt.

print();

}

TEST_F(MemoryBasedBtree, TestB
){
using namespace utec::memory;

btree<int, 5> bt;
bt.insert(42);
bt.insert(19);
bt.insert(85);
bt.insert(27);
bt.

print();

bt.insert(63);
bt.insert(35);
bt.insert(18);
bt.

print();

bt.insert(99);
bt.

print();

bt.insert(87);
bt.

print();

bt.insert(21);
bt.

print();

bt.insert(76);
bt.

print();

bt.insert(54);
bt.

print();

bt.remove(85);
bt.

print();

bt.remove(87);
bt.

print();

bt.remove(63);
bt.

print();

bt.remove(19);
bt.

print();

bt.remove(21);
bt.

print();

bt.remove(35);
bt.

print();

bt.remove(54);
bt.

print();

bt.remove(76);  //si quieres ver un merge with parent caso izquierda
bt.

print();

bt.remove(42);  //si quieres ver un merge leaf
bt.

print();

bt.remove(99);
bt.

print();

bt.remove(18);
bt.

print();

}

TEST_F(MemoryBasedBtree, TestC
) {
using namespace utec::memory;
btree<char> bt;
std::string abc = "qruvefghicdlmnojkwxyzabpst";
for (
int i = 0;
i<abc.

size();

++i) {
bt.
insert(abc[i]);
bt.

print();

}

for (
int i = abc.size() - 1;
i >= 0; --i) {
std::cout << "DELETE "<<abc[i]<<'\n';
bt.
remove(abc[i]);
bt.

print();

}
}

TEST_F(MemoryBasedBtree, TestD
) {
using namespace utec::memory;
btree<int> bt;
for(
int i = 0;
i < 10; i++)
bt.
insert(i);
bt.

print();

bt.remove(6);
bt.

print();

}

TEST_F(MemoryBasedBtree, TestE
) {

using namespace utec::memory;
btree<int> bt;
int input[24] = {16, 11, 17, 14, 23, 12, 1, 9, 19, 22, 7, 24, 6, 18, 8, 4, 21, 2, 10, 13, 15, 5, 20, 3};

for(
int i = 0;
i < 24; i++) {
bt.
insert(input[i]);
bt.

print();

}

for(
int i = 23;
i >= 0; i--) {
std::cout << "ELIMINARE " << input[i]<<"\n";
bt.
remove(input[i]);
bt.

print();

}

}

