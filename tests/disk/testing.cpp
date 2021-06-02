//
// Created by jaoks on 5/17/20.
//

#include "gtest/gtest.h"
#include "../../src/utec/disk/btree.h"
#include "../../src/utec/disk/pagemanager.h"
#include <iostream>

struct Pair {
    long id;
    long page_id;

    bool operator<(const Pair &p) const {
        if (this->id < p.id) return true;
        else return false;
    }

    bool operator<=(const Pair &p) const {
        if (this->id <= p.id) return true;
        else return false;
    }

    bool operator==(const Pair &p) const {
        if (this->id == p.id) return true;
        else return false;
    }
};

struct Student {
    long id;
    bool passed;
    char name[32];
    char surname[32];
    int n_credits;
};

std::ostream &operator<<(std::ostream &out, Pair &p) {
    out << p.id;
    return out;
}

#define PAGE_SIZE  1024

// Other examples:
// PAGE_SIZE 1024 bytes => 1Kb
// PAGE_SIZE 1024*1024 bytes => 1Mb
// 3 * sizeof(long)  =  count+right+page_id
// PAGE_SIZE = 3 * sizeof(long) +  (BTREE_ORDER + 1) * sizeof(T) + (BTREE_ORDER + 2) * sizeof(long)


#define BTREE_ORDER   ((PAGE_SIZE - (5 * sizeof(long) + sizeof(Pair)) ) /  (sizeof(Pair) + sizeof(long)))

using namespace utec::disk;

struct DiskBasedBtree : public ::testing::Test {
};

TEST_F(DiskBasedBtree, TestC) {

    /*std::shared_ptr<pagemanager> pm = std::make_shared<pagemanager>("b+tree.index", true);
    btree< Pair, BTREE_ORDER> bt(pm);
    pagemanager record_manager ("students.bin", true);
    long page_id;

    for(int i = 1; i <= 1000000; i++) {
         Student p { i * 10, true, "amaru", "orihuela", 32};
         page_id = i;
         record_manager.save(page_id, p); //principal
         bt.insert(Pair{p.id, page_id});  //index file
     }

    auto iter = bt.find(Pair{500000, -1});
    auto end = bt.find(Pair{500420, -1});
    for(; iter != end; ++iter) {
        auto pair = *iter;
        Student s;
        record_manager.recover(pair.page_id, s);
        std::cout<< "ID "<< s.id <<  '\n';
    }*/

}

TEST_F(DiskBasedBtree, TestC2) {

    /*std::shared_ptr <pagemanager> pm = std::make_shared<pagemanager>("b+tree.index", false);
    btree<Pair, BTREE_ORDER> bt(pm);
    pagemanager record_manager("students.bin", false);
    long page_id;

    auto iter = bt.find(Pair{9990, -1});
    auto end = bt.find(Pair{11990, -1});
    for(; iter !=end;++iter) {
        auto pair = *iter;
        Student s;
        record_manager.recover(pair.page_id, s);
        std::cout<< s.id << " " << s.name<< " " << s.surname <<  '\n';
    }*/

}

struct Computer {
    int id;
    int price;
    int speed;
    int hd;
    int ram;
    int screen;
    char cd[4];
    char multi[4];
    char premium[4];
    int ads;
    int trend;
};

TEST_F(DiskBasedBtree, TestC3 ) {
    std::cout << "TEST 1 : INSERT" << '\n';
    std::fstream in;
    in.open("Computers.csv", std::ios::in);
    if(!in.is_open()) {
        std::cout << "is not opened" << '\n';
        FAIL();
    }

    std::shared_ptr <pagemanager> pm = std::make_shared<pagemanager>("b+tree.index", true);
    btree<Pair, BTREE_ORDER> bt(pm);

    pagemanager record_manager("computers.bin", true);
    std::string line;
    getline(in, line);
    Computer record;
    int i = 1;
    while (getline(in, line)) {
        int pos_in_line = 0;
        std::vector<std::string> tokens;
        std::string token;
        while((pos_in_line=line.find(',')) != std::string::npos) {
            token = line.substr(0, pos_in_line);
            line.erase(0, pos_in_line+1);
            tokens.push_back(token);
        }
        tokens[0].erase(remove(tokens[0].begin(), tokens[0].end(), '\"'), tokens[0].end());
        record.id = stoi(tokens[0]);
        record.price = stoi(tokens[1]);
        record.speed = stoi(tokens[2]);
        record.hd = stoi(tokens[3]);
        record.ram = stoi(tokens[4]);
        record.screen = stoi(tokens[5]);
        tokens[6].erase(remove(tokens[6].begin(), tokens[6].end(), '\"'), tokens[6].end());
        strcpy(record.cd, tokens[6].c_str());
        tokens[7].erase(remove(tokens[7].begin(), tokens[7].end(), '\"'), tokens[7].end());
        strcpy(record.multi, tokens[7].c_str());
        tokens[8].erase(remove(tokens[8].begin(), tokens[8].end(), '\"'), tokens[8].end());
        strcpy(record.premium, tokens[8].c_str());
        record.ads = stoi(tokens[9]);
        record.trend = stoi(token);

        record_manager.save(i, record);
        bt.insert(Pair{record.id, i});
        i++;
    }
}

TEST_F(DiskBasedBtree, TestC4 ) {
    std::shared_ptr <pagemanager> pm = std::make_shared<pagemanager>("b+tree.index", false);
    btree<Pair, BTREE_ORDER> bt(pm);

    pagemanager record_manager("computers.bin", false);

    auto iter = bt.find(Pair{1420, -1});
    auto pair = *iter;
    Computer record;
    record_manager.recover(pair.page_id, record);
    std::cout << "ID: "<< record.id << std::endl;
    std::cout << "PRICE: "<< record.price << std::endl;
    std::cout << "SPEED: " << record.speed << std::endl;
    std::cout << "HD: " << record.hd << std::endl;
    std::cout << "RAM: " << record.ram << std::endl;
}

TEST_F(DiskBasedBtree, TestC5) {
    std::shared_ptr <pagemanager> pm = std::make_shared<pagemanager>("b+tree.index", false);
    btree<Pair, BTREE_ORDER> bt(pm);

    pagemanager record_manager("computers.bin", false);

    auto iter = bt.find(Pair{394, -1});
    auto end = bt.find(Pair{5120, -1});
    for(; iter != end; ++iter) {
        auto pair = *iter;
        Computer record;
        record_manager.recover(pair.page_id, record);
        std::cout << "ID: "<< record.id << " - ";
        std::cout << "PRICE: "<< record.price << " - ";
        std::cout << "SPEED: " << record.speed << " - ";
        std::cout << "HD: " << record.hd << " - ";
        std::cout << "RAM: " << record.ram << std::endl;
    }
}