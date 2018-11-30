#pragma once
#include "../Sharing/asminstructions.hpp"

namespace asmcmplr {
	size_t 
		sizeof_instruction(std::string const&);

	share::asmcom::num_t
		num_of_instruction(std::string const& inst);

    size_t
        get_argscount(std::string const& inst);

    const share::asmcom::arg_t*
        get_args(std::string const& inst);

}
