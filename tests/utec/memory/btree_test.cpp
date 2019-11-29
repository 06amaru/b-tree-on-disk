
// #include <utecdf/column/column.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utec/disk/btree.hpp>
#include <utec/disk/pagemanager.hpp>

#include <fmt/core.h>

// PAGE_SIZE 64 bytes
struct Pair {
    long id;
    long page_id;
    bool operator <(const Pair &p) const {
        if ( this->id < p.id) return true;
        else return false;
    }
    bool operator <=(const Pair &p) const {
        if (this->id <= p.id) return true;
        else return false;
    }
    bool operator ==(const Pair &p) const {
        if (this->id == p.id) return true;
        else return false;
    }
};

struct Student {
    long  id;
    bool passed;
    char name[32];
    char surname[32];
    int  n_credits;
};

std::ostream& operator <<(std::ostream& out, Pair &p)  {
      out << p.id;
      return out;
}

#define PAGE_SIZE  1024 

// Other examples:
// PAGE_SIZE 1024 bytes => 1Kb
// PAGE_SIZE 1024*1024 bytes => 1Mb

// PAGE_SIZE = 4 * sizeof(long) +  (BTREE_ORDER + 1) * sizeof(int) + (BTREE_ORDER + 2) * sizeof(long)  
// PAGE_SIZE = 4 * sizeof(long) +  (BTREE_ORDER) * sizeof(int) + sizeof(int) + (BTREE_ORDER) * sizeof(long) + 2 * sizeof(long)
// PAGE_SIZE = (BTREE_ORDER) * (sizeof(int) + sizeof(long))  + 4 * sizeof(long) + sizeof(int) +  2 * sizeof(long)
//  BTREE_ORDER = PAGE_SIZE - (4 * sizeof(long) + sizeof(int) +  2 * sizeof(long)) /  (sizeof(int) + sizeof(long))

#define BTREE_ORDER   ((PAGE_SIZE - (6 * sizeof(long) + sizeof(Pair)) ) /  (sizeof(Pair) + sizeof(long)))

using namespace utec::disk;
    
struct DiskBasedBtree : public ::testing::Test
{
};


 TEST_F(DiskBasedBtree, TestA) {  //if you run this then change the page size ( Pair -> int)
     std::shared_ptr<pagemanager> pm = std::make_shared<pagemanager>("btree.index", true);
     std::cout << "PAGE_SIZE: " << PAGE_SIZE << std::endl;
     std::cout << "BTREE_ORDER: " << BTREE_ORDER << std::endl;
     btree<char, 3> bt(pm);
     std::string values = "zxcnmvafjdaqpirue";
     for(auto c : values) {
        std::cout<<"INSERTANDO "<< c <<'\n';
        bt.insert(c);
        bt.print_tree();
     }
     std::cout<<"FIN DEL INSERT"<<'\n';
     bt.print_tree();
    
     // btree<int,BTREE_ORDER>::iterator it = bt.begin();
     btree<char,3>::iterator it = bt.find('f');
     btree<char,3>::iterator end = bt.find('m');
     for( ; it != end; ++it){
         std::cout<<(char)*it<<' ';
     }
    
 }
 TEST_F(DiskBasedBtree, TestB) {
    std::shared_ptr<pagemanager> pm = std::make_shared<pagemanager>("btree2.index", true);
    btree<char,4> bt(pm);
    std::string abc = "qruvefghicdlmnojkwxyzabpst";
    for (int i = 0; i < abc.size(); ++i) {
        bt.insert(abc[i]);
//        bt.print_tree();
    }

    for (int i = abc.size()-1; i >= 0; --i) {
        std::cout << "DELETE "<<abc[i]<<'\n';
        bt.remove(abc[i]);
        bt.print_tree();
    }

 }
 
//TEST_F(DiskBasedBtree, TestC) {
//
//    std::shared_ptr<pagemanager> pm = std::make_shared<pagemanager>("b+tree.index", true);
//    btree< Pair, BTREE_ORDER> bt(pm);
//    pagemanager record_manager ("students.bin", true);
//    long page_id;
//
//    for(int i = 1; i < 1000000; i++) {
//         Student p { i * 10, true, "amaru", "orihuela", 32};
//         page_id = i;
//         record_manager.save(page_id, p);
//         bt.insert(Pair{p.id, page_id});
//     }
//
//    auto iter = bt.find(Pair{500000, -1});
//    bt.print();
//    auto end = bt.find(Pair{500420, -1});
//    for(; iter != end; ++iter) {
//        auto pair = *iter;
//        Student s;
//        record_manager.recover(pair.page_id, s);
//        std::cout<< s.id <<  '\n';
//    }
//
//}
//
//TEST_F(DiskBasedBtree, TestC2) {
//
//    std::shared_ptr<pagemanager> pm = std::make_shared<pagemanager>("b+tree.index", false);
//    btree< Pair, BTREE_ORDER> bt(pm);
//    pagemanager record_manager ("students.bin", false);
//    long page_id;
//
//    auto iter = bt.find(Pair{9990, -1});
//    auto end = bt.find(Pair{11990, -1});
//    for(; iter != end; ++iter) {
//        auto pair = *iter;
//        Student s;
//        record_manager.recover(pair.page_id, s);
//        std::cout<< s.id <<  '\n';
//    }
//
//}

