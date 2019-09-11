#ifndef HELPER_H
#define HELPER_H

#include "instr.h"

#include <functional>

namespace help {
using ret_f = std::function<void(const char*)>;

ret_f insn_to_exfunc(unsigned char num) ;
size_t sizeof_instr_args(unsigned char num);

}

#endif // HELPER_H
