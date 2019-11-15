#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace utec {
namespace disk {

class pagemanager : protected std::fstream {
public:
  pagemanager(std::string file_name, bool trunc = false);

  ~pagemanager();

  inline bool is_empty() { return empty; }

  template <class Register> void save(const long &n, Register &reg) {
    clear();
    seekp(n * sizeof(Register), std::ios::beg);
    write(reinterpret_cast<const char *>(&reg), sizeof(reg));
  }

  // template<class Register>
  // long save(Register &reg)
  // {
  //     clear();
  //     seekp(0, std::ios::end);
  //     write(reinterpret_cast<const char *> (&reg), sizeof(reg));

  //     return page_id_count - 1;
  // }

  template <class Register> bool recover(const long &n, Register &reg) {
    clear();
    seekg(n * sizeof(Register), std::ios::beg);
    read(reinterpret_cast<char *>(&reg), sizeof(reg));
    return gcount() > 0;
  }

  // Marca el registro como borrado:

  template <class Register> void erase(const long &n) {
    char mark;
    clear();
    mark = 'N';
    seekg(n * sizeof(Register), std::ios::beg);
    write(&mark, 1);
  }

private:
  std::string fileName;
  int pageSize;
  bool empty;
  long page_id_count;
};
} // namespace disk
} // namespace utec