#include "pagemanager.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace utec {
namespace disk {

pagemanager::pagemanager(std::string file_name, bool trunc)
    : std::fstream(file_name.data(),
                   std::ios::in | std::ios::out | std::ios::binary) {
  empty = false;
  fileName = file_name;
  if (!good() || trunc) {
    empty = true;
    open(file_name.data(),
         std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
  }
}

pagemanager::~pagemanager() { close(); }

} // namespace disk
} // namespace utec