#include "asmcompiler.hpp"
#include "asmcompinfo.hpp"
#include <algorithm>
#include <sstream>
#include <vector>

void push_if_important(asmcmplr::codelist_t& codelist, const char* from, const char* until) noexcept {
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

asmcmplr::codelist_t
asmcmplr::splitcode(char* code, std::streamsize size) noexcept {
	asmcmplr::codelist_t codelist;
	for (int i = 0, prev = 0; i < size; ++i) {
		if (code[i] == '\n') {
			push_if_important(codelist, code + prev, code + i);
			prev = i + 1;
		}
	}
	return codelist;
}

bool has_whitespace(std::string const& str) noexcept {
	for (auto const& i : str)
		if (i == ' ' || i == '\t')
			return true;
	return false;
}


std::pair<asmcmplr::funcslist_t, asmcmplr::markslist_t>
asmcmplr::markswork(asmcmplr::codelist_t& code) {
    asmcmplr::position_t position = 0;
	asmcmplr::funcslist_t funcslist;
	asmcmplr::markslist_t markslist;
	for (auto& item : code) {
		if (*item.begin() == '.') { //it's a function
			std::string funcname(item.begin() + 1, item.end());
			if (has_whitespace(funcname)) {
				throw std::runtime_error("Incorrect name function: " + funcname);
			}
			if (funcslist.count(funcname)) {
				throw std::runtime_error("Ambiguous function name:" + funcname);
			}
			funcslist[funcname] = position;
		}
		else if (*item.rend() == ':') { // it's a mark
			std::string markname(item.begin() + 1, item.end());
			if (has_whitespace(markname)) {
				throw std::runtime_error("Incorrect name function: " + markname);
			}
			if (funcslist.count(markname)) {
				throw std::runtime_error("Ambiguous function name:" + markname);
			}
			funcslist[markname] = position;
		}
		else {
			position += sizeof_instruction(item);
		}
	}

	std::remove_if(code.begin(), code.end(), [](std::string const& str) noexcept {
		if (*str.begin() == '.') // it's a function
			return true;
        if (*str.rend() == ':') //it's a mark
			return true;
		return false;
	});

	return { funcslist, markslist };
}

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
std::string to_binary(T const& val) noexcept {
	std::string ans;
	char*p = (char*)&val;
    for (unsigned int i = 0u; i < sizeof(T); ++i) {
		ans.push_back(p[i]);
	}
	return ans;
}

struct arg_info {share::asmcom::arg_t type; std::string value;};
std::tuple<share::asmcom::name_t, std::vector<arg_info>> parse_instr_line(std::string line) {
    char* buf = new char[line.size()];
    size_t pos = 0;
    for(int i = 0; line[pos] != ' ' && pos < line.size(); ++i, ++pos) {
        buf[i] = line[pos];
    }
    std::string name(line.c_str(), pos);
    std::vector<arg_info> args;
    const share::asmcom::arg_t* argtype = asmcmplr::get_args(name);

    for(size_t j = 0; j != asmcmplr::get_argscount(name); ++j) {
        size_t i = 0;
        for(; line[pos] != ' ' && pos < line.size(); pos++);
        for(; (line[pos] != ' ' || line[pos] != ',') && pos < line.size(); ++i, ++pos) {
            buf[i] = line[pos];
        }
        if(j + 1 != line.size())
            if(line[pos] != ',')
                throw std::runtime_error("Incorrect syntex. Expect \',\' in the: " + line);

        args.push_back({ argtype[j], std::string(buf, i) });
    }

    return std::tuple<share::asmcom::name_t, std::vector<arg_info>>{ name, args };
}



std::string
asmcmplr::translate(asmcmplr::codelist_t const& code, asmcmplr::funcslist_t const& funclist, asmcmplr::markslist_t const& markslist) {
	std::string ans;
    ans.append(to_binary<const unsigned short>(funclist.at("main")));

	for (auto const& item : code) {
        std::string name;
        std::vector<arg_info> args;
        std::tie(name, args) = parse_instr_line(item);

        ans.append(to_binary(num_of_instruction(name)));
        for (auto &arg : args) {
            switch (arg.type)
			{
            case share::type::INT: {
                share::int_t i_arg = share::int_t(std::stoi(arg.value));
                ans.append(to_binary(i_arg));
                break;
            }
            case share::type::FLOAT: {
                share::float_t f_arg = share::float_t(std::stod(arg.value));
                ans.append(to_binary(f_arg));
                break;
            }
            case share::type::FUNC: {
                std::string func_arg = arg.value;
                ans.append(to_binary(funclist.at(func_arg)));
                break;
            }
            case share::type::MARK: {
                std::string mark_arg = arg.value;
                ans.append(to_binary(markslist.at(mark_arg)));
                break;
            }
			default:
                throw std::runtime_error("Unworked type (num: " + std::to_string(int(arg.type)) + "). For more information read Share/asminstructions.hpp");
			}
		}
	}
    return ans;
}
