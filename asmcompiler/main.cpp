#include <iostream>
#include <fstream>
#include <list>
#include "asmcompiler.hpp"

std::streamsize file__size(std::fstream& file) noexcept {
	file.seekg(std::ios::end);
	auto size = file.tellg();
	file.seekg(std::ios::beg);
    return std::streamsize(size);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Enter " << argv[0] << " [file name]\n";
		return 0;
	}
    const char* outfile = "main.bc";
	
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

    auto size = file__size(file);
	char* readeblecode = new char[size];
	file.read(readeblecode, size);
	file.close();

	std::list<std::string> codelist = asmcmplr::splitcode(readeblecode, size);
    asmcmplr::funcslist_t funcs; asmcmplr::markslist_t marks;
    std::tie(funcs, marks) = asmcmplr::markswork(codelist); // find & erase marks

    if (!funcs.count("main")) {
        throw std::runtime_error("Not found a main function");
    }

	std::string bytecode = asmcmplr::translate(codelist, funcs, marks);
    outf.write(bytecode.c_str(), std::streamsize(bytecode.size()));
	outf.close();

	delete[] readeblecode;
}
