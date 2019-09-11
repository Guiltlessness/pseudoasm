#ifndef ASMCOMP_H
#define ASMCOMP_H

#include <string>
#include <map>
#include <list>


namespace  asmcomp {
using position_t = unsigned short;
using markslist_t = std::map<std::string, position_t>;
using funcslist_t = std::map<std::string, position_t>;
using codelist_t = std::list<std::string>;

std::pair<funcslist_t, markslist_t>
    markswork(codelist_t& code);

std::string
    translate(codelist_t const& code, funcslist_t const& funclist, markslist_t const& markslist);

asmcomp::codelist_t
    splitcode(char* readeblecode, std::streamsize size) noexcept;
}


#endif // ASMCOMP_H
