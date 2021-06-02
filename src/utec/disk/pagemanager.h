//
// Created by jaoks on 5/17/20.
//

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#ifndef B_TREE_V2_0_PAGEMANAGER_H
#define B_TREE_V2_0_PAGEMANAGER_H

namespace utec {
    namespace disk {
        class pagemanager : protected std::fstream {
        public:
            //in : file open for reading
            //out : file open for writing
            //binary : operations are performed in binary mode rather than text
            pagemanager(std::string file_name, bool trunc = false) : std::fstream(file_name.data(),
                                                                                  std::ios::in | std::ios::out |
                                                                                  std::ios::binary) {
                empty = false;
                fileName = file_name;
                if (!good() || trunc) {
                    empty = true;
                    //trunc : any contents that existed in the file before it is open are discarded
                    open(file_name.data(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
                }
            }

            ~pagemanager() {
                close();
            };

            inline bool is_empty() { return empty; }

            template<class Register>
            void save(const long &n, Register &reg) {
                clear();
                //off: offset value
                //way: object of type ios_base::seekdir (begin current or end)
                seekp(n * sizeof(Register), std::ios::beg);
                //pointer to an array of at least n characters
                //reinterpret_cast convert any pointer object to char pointer (work with bits)
                write(reinterpret_cast<char *>(&reg), sizeof(reg));
            }

            template<class Register>
            bool recover(const long &n, Register &reg) {
                clear();
                seekg(n * sizeof(Register), std::ios::beg);
                read(reinterpret_cast<char *>(&reg), sizeof(reg));
                return gcount() > 0;
            }

            template<class Register>
            void erase(const long &n) {
                clear();
                char mark = 'N';
                seekg(n * sizeof(Register), std::ios::beg);
                write(&mark, 1);
            }

        private:
            std::string fileName;
            int pageSize;
            bool empty;
            long page_id_count;
        };
    }
}

#endif //B_TREE_V2_0_PAGEMANAGER_H
