#ifndef INSTR_H
#define INSTR_H

#include <string>
#include <functional>
#include <tuple>

namespace AllWhatYouWantToKnowAboutInstr {
using pos_t = unsigned short;
using nreg_t = unsigned char;
using int_t = int;
using float_t = float;
using str_t = struct { int pos; };
using fpos_t = struct { pos_t pos; };
using mpos_t = struct { pos_t pos; };

enum instr_kind {
    USUAL,
    FUNCCALLABLE,
    MARKLEAPING
};

struct instr_t {
    std::string name;
    std::function<void(const char*)> exec;
    std::function<std::string(const char*)> get_bytes_args;
    std::size_t argssize;
    instr_kind kind;

    instr_t(std::string const&s,
            std::function<void(const char*)> const& e,
            std::function<std::string(const char*)> const& g,
            std::size_t const& as,
            instr_kind k);
    instr_t();
    ~instr_t();
};


class list {
    std::size_t count_instrs;
    instr_t* instrs;
public:
    list();
    ~list();
    instr_t* const& get_list() const noexcept;
    std::size_t get_list_size() const noexcept;
};

}

#endif // INSTR_H
