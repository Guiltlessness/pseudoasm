#include "asmcompinfo.hpp"
#include <unordered_map>
#include <utility>

using info = struct {
    const share::asmcom::arg_t* args;
    const share::asmcom::num_t num;
    const share::asmcom::argcount_t argcount;
};

const std::unordered_map<std::string, info> init_to_instrsize() noexcept {
	std::unordered_map<std::string, info> ans;
    for (size_t i = 0; i < sizeof(share::comands) / sizeof(share::asmcom); ++i) {
        ans.insert({ share::comands[i].name, {share::comands[i].funarg, share::comands[i].num, share::comands[i].argcount } });
	}
	return ans;
}

const std::unordered_map<std::string, info> to_instrsize = init_to_instrsize();

size_t asmcmplr::sizeof_instruction(std::string const& inst) {
	if (!to_instrsize.count(inst))
		throw std::runtime_error("No found the instruction name " + inst);
    size_t ans = 1; // a byte of instr
    const share::asmcom::arg_t* args = to_instrsize.at(inst).args;
    for (int i = 0; i != to_instrsize.at(inst).argcount; ++i) {
        switch (args[i]) {
        case share::type::INT:
            ans += sizeof (share::int_t);
            break;
        case share::type::FLOAT:
            ans += sizeof (share::float_t);
            break;
        case share::type::MARK:
            ans += sizeof (share::mark_t);
            break;
        case share::type::FUNC:
            ans += sizeof (share::func_t);
            break;
        case share::type::NONE:
            break;
        }
    }
    return ans;
}

share::asmcom::num_t
asmcmplr::num_of_instruction(std::string const& inst) {
	if (!to_instrsize.count(inst))
		throw std::runtime_error("No found the instruction name " + inst);
	return to_instrsize.at(inst).num;
}

const share::asmcom::arg_t*
asmcmplr::get_args(std::string const& inst) {
    if (!to_instrsize.count(inst))
        throw std::runtime_error("No found the instruction name " + inst);
    return to_instrsize.at(inst).args;
}

size_t
asmcmplr::get_argscount(std::string const& inst) {
    if (!to_instrsize.count(inst))
        throw std::runtime_error("No found the instruction name " + inst);
    return to_instrsize.at(inst).argcount;
}
