#pragma once
#include <map>
#include <string>
#include <list>

namespace asmcmplr {
	using markslist_t = std::map<std::string, int>;
	using funcslist_t = std::map<std::string, int>;
	using codelist_t = std::list<std::string>;

	std::pair<funcslist_t, markslist_t> 
		markswork(codelist_t& code, size_t size) noexcept;

	char*
		translate(codelist_t const& code, funcslist_t const& funclist) noexcept;

	asmcmplr::codelist_t
		splitcode(char* readeblecode) noexcept;

	void
		valid_flist(funcslist_t const& flist);

	void
		valid_mlist(markslist_t const& mlist);
}