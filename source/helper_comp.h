#ifndef HELPER_COMP_H
#define HELPER_COMP_H

#include "instr.h"

namespace help {
using argp_f = std::function<std::string(const char*)>;

unsigned char num_from_name(std::string const& name);

size_t sizeof_instr_args(unsigned char num);

argp_f argparser_from_num(unsigned char num);

bool func_callable(std::string const&);
bool mark_leaping(std::string const&);

}

#endif // HELPER_COMP_H
