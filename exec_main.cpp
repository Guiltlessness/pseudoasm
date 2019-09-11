#include <iostream>
#include <fstream>
#include "source/executor.h"

extern comp::executor vm;

std::streamsize file__size(std::fstream& file) noexcept {
    auto size1 = file.tellg();
    file.seekg(0, std::ios::end);
    auto size2 = file.tellg();
    file.seekg(0, std::ios::beg);
    return std::streamsize(size2 - size1);
}

using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Enter " << argv[0] << " [file name]\n";
        return 0;
    }

    fstream file(argv[1], ios::in);
    if (!file.is_open()) {
        cout << "Cannot open file\n";
        return 1;
    }
    auto size = file__size(file);

    if (size < int(2 * sizeof(AllWhatYouWantToKnowAboutInstr::pos_t))) // if less than the lenght of a header
        return 0;

    auto codestr = new char[size_t(size)];
    file.read(codestr, size);
    vm.set_code(codestr);
    vm.run();

    delete[] codestr;
    return 0;
}
