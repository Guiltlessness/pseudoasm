#include <iostream>
#include <fstream>
#include <list>
#include "asmcompiler.hpp"

size_t file__size(std::fstream& file) noexcept {
	file.seekg(std::ios::end);
	auto size = file.tellg();
	file.seekg(std::ios::beg);
	return size;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Enter " << argv[0] << " [file name]\n";
		return 0;
	}
	char* outfile = argc == 3 ? argv[2] : "b.out";
	
	std::fstream file(argv[1], std::ios::in | std::ios::binary);
	std::fstream outf(outfile, std::ios::out | std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Cannot open file " << argv[1] << std::endl;
		return 1;
	}

	if (!outf.is_open()) {
		std::cerr << "Cannot open(create) file " << outfile << std::endl;
		return 1;
	}

	size_t size = file__size(file);
	char* readeblecode = new char[size];
	file.read(readeblecode, size);
	file.close();

	std::list<std::string> codelist = asmcmplr::splitcode(readeblecode);
	auto [funcs, marks] = asmcmplr::markswork(codelist, size); // find & erase marks
	asmcmplr::valid_flist(funcs);
	asmcmplr::valid_mlist(marks);
	std::string bytecode = asmcmplr::translate(codelist, funcs);
	outf.write(bytecode.c_str(), bytecode.size());
	outf.close();

	delete[] readeblecode;
}