#include <functional>

namespace share {
        using int_t = int;
        using float_t = float;
        using mark_t = unsigned short;
        using func_t = unsigned short;

        enum class type {
            NONE = 0,
            INT = 1,
            FLOAT = 2,
            MARK = 3,
            FUNC = 4,
        };

	struct asmcom {
		using name_t = std::string;
		using num_t = unsigned char;
		using com_t = std::function<void(char*)>;
                using arg_t = type;
                using argcount_t = unsigned char;

		name_t name;
		num_t num;
                argcount_t argcount;
                arg_t funarg[2];
		com_t com;
	};

	extern const asmcom* comands;
}
