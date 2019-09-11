#include "helper_exec.h"
#include "helper_comp.h"
#include "instr.h"

#include <map>
#include <sstream>
#include <iostream>



namespace help {
using map4num_t = std::map<unsigned char, AllWhatYouWantToKnowAboutInstr::instr_t>;
using map4str_t = std::map<std::string, unsigned char>;

const AllWhatYouWantToKnowAboutInstr::list instrlist;

map4num_t init_map4num() noexcept {
    map4num_t ans;
    auto const& list = instrlist.get_list();
    for (unsigned char i = 0; i < instrlist.get_list_size(); ++i) {
        ans.insert({ i, list[i] });
    }
    return ans;
};

map4str_t init_map4str() noexcept {
    map4str_t ans;
    auto const& list = instrlist.get_list();
    for (unsigned char i = 0; i < instrlist.get_list_size(); ++i) {
        ans.insert({ list[i].name, i });
    }
    return ans;
}

const map4num_t map4num = init_map4num();
const map4str_t map4str = init_map4str();

ret_f insn_to_exfunc(unsigned char num) {
    if (!map4num.count(num)) {
        std::stringstream s;
        for(auto const& i : map4num)
            s << i.first << std::endl;
        throw std::runtime_error(std::to_string(num) + " haven't been found in the instr list\nThe supported instructions:\n" + s.str());
    }
    return map4num.at(num).exec;
}

size_t sizeof_instr_args(unsigned char num) {
    if (!map4num.count(num)) {
        std::stringstream s;
        for(auto const& i : map4num)
            s << i.first << std::endl;
        throw std::runtime_error(std::to_string(num) + " haven't been found in the instr list\nThe supported instructions:\n" + s.str());
    }
    return map4num.at(num).argssize;
}

unsigned char num_from_name(std::string const& name)  {
    if (!map4str.count(name)) {
        std::stringstream s;
        for(auto const& i : map4str)
            s << i.first << std::endl;
        throw std::runtime_error(name + " haven't been found in the instr list\nThe supported instructions:\n" + s.str());
    }
    return map4str.at(name);
}

argp_f argparser_from_num(unsigned char num)  {
    if (!map4num.count(num)) {
        std::stringstream s;
        for(auto const& i : map4num)
            s << i.first << std::endl;
        throw std::runtime_error(std::to_string(num) + " haven't been found in the instr list\nThe supported instructions:\n" + s.str());
    }
    return map4num.at(num).get_bytes_args;
}
\
bool func_callable(std::string const& name)  {
    if (!map4str.count(name)) {
        std::stringstream s;
        for(auto const& i : map4str)
            s << i.first << std::endl;
        throw std::runtime_error(name + " haven't been found in the instr list\nThe supported instructions:\n" + s.str());
    }
    auto n = map4str.at(name);
    auto const& instr = map4num.at(n);
    return instr.kind == AllWhatYouWantToKnowAboutInstr::instr_kind::FUNCCALLABLE;
}

bool mark_leaping(std::string const& name)  {
    if (!map4str.count(name)) {
        std::stringstream s;
        for(auto const& i : map4str)
            s << i.first << std::endl;
        throw std::runtime_error(name + " haven't been found in the instr list\nThe supported instructions:\n" + s.str());
    }
    auto n = map4str.at(name);
    auto const& instr = map4num.at(n);
    return instr.kind == AllWhatYouWantToKnowAboutInstr::instr_kind::MARKLEAPING;
}
}
