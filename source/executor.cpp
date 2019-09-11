#include "executor.h"
#include "helper_exec.h"

#include <iostream>
#include<functional>

comp::executor::executor() noexcept
{}


void comp::executor::set_code(const char* fcode) noexcept {
    pos = *(const comp::executor::pos_t*)fcode;
    unvarmem = *(const comp::executor::pos_t*)(fcode + sizeof(comp::executor::pos_t)) + fcode;
    fcode += 2 * sizeof (comp::executor::pos_t);
    code = fcode;
    is_on = true;
}

void comp::executor::run() noexcept {
    std::function<void(const char*)> cur_instr;
    callstack.push( pos ); // main function
    while(is_on) {
        unsigned char ninstr = *(const unsigned char*)(code + pos);
        cur_instr = help::insn_to_exfunc(ninstr);
        auto buf = pos += sizeof (unsigned char);
        pos += help::sizeof_instr_args(ninstr);
        cur_instr(code + buf);
    }
}
