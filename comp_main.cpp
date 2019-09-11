#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include "source/asmcomp.h"

std::size_t file__size(std::fstream& file) noexcept {
    file.seekg(0, std::ios::beg);
    auto size1 = file.tellg();
    file.seekg(0, std::ios::end);
    auto size2 = file.tellg();
    file.seekg(0, std::ios::beg);
    return std::size_t(size2 - size1);
}

std::list<std::string> get_codelist(char** files, int count) {
    std::list<std::string> codelist;
    for (int i = 0; i < count; ++i) {
        std::fstream file(files[i], std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            throw("Cannot open file " + std::string(files[i]));
        }
        auto size = file__size(file);
        char* readeblecode = new char[size_t(size)];
        file.read(readeblecode, int(size));
        file.close();
        auto additive = asmcomp::splitcode(readeblecode, size);
        codelist.insert(codelist.end(), additive.begin(), additive.end());
        asmcomp::splitcode(readeblecode, size);
        delete[] readeblecode;
    }
    return codelist;
}

int main(int argc, char** argv) {
//    int argc = 2;
//    char* argv[] = {
//        "main",
//        "/home/creator/Documents/EntC/untitled5/fact.mc"
//    };
    if (argc < 2) {
        std::cerr << "Enter " << argv[0] << " [file name]\n";
        return 0;
    }
    const char* outfile = "main.bc";

    std::fstream file(argv[1], std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open file " << argv[1] << std::endl;
        return 1;
    }

    std::list<std::string> codelist = get_codelist(argv + 1, argc - 1);
    asmcomp::funcslist_t funcs; asmcomp::markslist_t marks;
    std::tie(funcs, marks) = asmcomp::markswork(codelist); // find & erase marks


    if (!funcs.count("main")) {
        throw std::runtime_error("Not found a main function " );
        return 1;
    }

    std::string bytecode = asmcomp::translate(codelist, funcs, marks);

    std::fstream outf(outfile, std::ios::out | std::ios::binary);
    if (!outf.is_open()) {
        std::cerr << "Cannot open(create) file " << outfile << std::endl;
        return 1;
    }
    outf.write(bytecode.c_str(), std::streamsize(bytecode.size()));
    outf.close();
}
