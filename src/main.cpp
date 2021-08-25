#include "chip.hpp"

#include <iostream>
#include <stdexcept>

void run(const char* rom_filename)
{
	try {
		cc8::Chip chip(rom_filename);
		chip.run();
	}
	catch (const std::exception& e) {
		std::cerr << "\x1b[1;91m[ERROR]\x1b[m " << e.what() << std::endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		throw std::runtime_error("Filename required as argument");
	}

	run(argv[1]);
}
