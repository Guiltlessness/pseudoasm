#include "asmcomp.h"
#include "helper_comp.h"

#include <vector>

extern std::string unval_mem_infuture;

namespace  {

void push_if_important(asmcomp::codelist_t& codelist, const char* from, const char* until) noexcept {
    for (; from < until; ++from) {
        if (!(from[0] == ' ' || from[0] == '\t'))
            break;
    }
    for (const char*p = from; p < until; ++p) {
        if (p[0] == '#') {
            until = p;
            break;
        }
    }
    if (until != from) {
        codelist.push_back(std::string(from, size_t(until - from)));
    }
}

template<typename T>
std::string to_binary(T const& val) noexcept {
    return std::string((const char*)&val, sizeof (T));
}

std::tuple<std::string, std::string> split_instr_line(std::string const& line) {
    size_t name_until = 0, arg_from = 0, arg_until = 0;
    for( ; name_until < line.size(); ++name_until)
        if (line[name_until] == ' ' || line[name_until] == '\t')
            break;
    for (arg_from = name_until; arg_from < line.size(); ++arg_from)
        if (!(line[arg_from] == ' ' || line[arg_from] == '\t'))
            break;
    for (arg_until = line.size() - 1; arg_until > arg_from; --arg_until)
        if (!(line[arg_until] == ' ' || line[arg_until] == '\t'))
            break;
    return std::make_tuple(std::string(line.c_str(), name_until), std::string(line.c_str() + arg_from, line.c_str() + arg_until + 1));
}

std::string str_if_valid(std::string const& val) {
    for(auto i :val) {
        if(i == ' ' || i == '\t')
            throw std::runtime_error("Invalid name of a mark/func");
    }
    if (val.front() == '.') {
        return std::string(val.begin() + 1, val.end());
    } else { // expect ':'
        return std::string(val.begin(), val.end() - 1);
    }
}

}

asmcomp::codelist_t
asmcomp::splitcode(char* code, std::streamsize size) noexcept {
    asmcomp::codelist_t codelist;
    for (int i = 0, prev = 0; i < size; ++i) {
        if (code[i] == '\n') {
            push_if_important(codelist, code + prev, code + i);
            prev = i + 1;
        }
    }
    return codelist;
}
#include <iostream>
std::string asmcomp::translate(codelist_t const& code,
                               funcslist_t const& funclist,
                               markslist_t const& markslist) {
    std::string ans;
    ans.append(to_binary(funclist.at("main")));

    std::size_t mem_ind = ans.size();
    ans.append(to_binary(asmcomp::position_t(0)));

    for(auto &line : code) {
        std::string name, args;
        std::tie(name, args) = split_instr_line(line);

        if (help::func_callable(name)) {
            args = std::to_string(funclist.at(args));
        }
        if (help::mark_leaping(name)) {
            args = std::to_string(markslist.at(args));
        }

        const unsigned char num_instr = help::num_from_name(name);
        ans.append(to_binary(num_instr));

        help::argp_f argsparser = help::argparser_from_num(num_instr);
        ans.append(argsparser(args.c_str()));
    }

    auto mem_pos = to_binary(asmcomp::position_t(ans.size()));
    for (auto i : mem_pos) {
        ans[ mem_ind++ ] = i;
    }
    ans.append(unval_mem_infuture);
    return ans;
}

std::pair<asmcomp::funcslist_t, asmcomp::markslist_t>
asmcomp::markswork(codelist_t& code) {
    asmcomp::funcslist_t flist;
    asmcomp::markslist_t mlist;

    std::string name;
    asmcomp::position_t pos = 0;
    std::vector<codelist_t::iterator> excess_lines;

    for(auto line = code.begin(); line != code.end(); ++line) {
        if (line->front() == '.') { // It's a function
            flist.insert({ str_if_valid(*line), pos });
            excess_lines.push_back(line);
        } else if (line->back() == ':') { // It's a mark
            mlist.insert({ str_if_valid(*line), pos });
            excess_lines.push_back(line);
        } else {
            std::tie(name, std::ignore) = split_instr_line(*line);
            const unsigned char num_instr = help::num_from_name(name);
            pos += help::sizeof_instr_args(num_instr) + sizeof (unsigned char);
        }
    }
    for (auto i : excess_lines) {
        code.erase(i);
    }
    return { flist, mlist };
}
