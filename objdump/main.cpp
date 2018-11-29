#include <iostream>
#include <fstream>

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


}