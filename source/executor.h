#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stack>
#include "instr.h"

namespace comp {
using nreg_t = AllWhatYouWantToKnowAboutInstr::nreg_t;
using int_t = AllWhatYouWantToKnowAboutInstr::int_t;
using float_t = AllWhatYouWantToKnowAboutInstr::float_t;
using str_t = AllWhatYouWantToKnowAboutInstr::str_t;


struct executor {
    struct type {
        union {
            int_t i;
            float_t f;
            str_t s;
        };
    };
    using pos_t = AllWhatYouWantToKnowAboutInstr::pos_t;
    using vstack_t = std::stack<type>;
    using callstack_t = std::stack<pos_t>;

    int_t regi[16];
    float_t regf[16];
    vstack_t valstack;
    callstack_t callstack;
    pos_t pos;
    bool is_on = false;
    const char * unvarmem;
    const char * code;

    executor() noexcept;
    void set_code(const char*) noexcept;
    void run() noexcept;
};

}
#endif // EXECUTOR_H
