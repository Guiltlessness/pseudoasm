#include <functional>

namespace share {
	struct asmcom {
		using num_t = unsigned char;
		using com_t = std::function<void(char*)>;
		using funarg_t = struct {

		};
		num_t num;
		funarg_t funarg;
		com_t com;
	};

	extern const asmcom* comands;
}