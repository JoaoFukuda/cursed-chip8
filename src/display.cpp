#include "display.hpp"

#include <unistd.h>
#include <cstdlib>
#include <iostream>

cc8::Display::Display()
{
	std::system("stty raw -echo");
	write(1, "\x1b[?25l", 6);
	write(1, "\x1b[?1049h", 8);
	write(1, "\x1b[s", 3);
}

cc8::Display::~Display()
{
	std::system("stty -raw echo");
	write(1, "\x1b[u", 3);
	write(1, "\x1b[?1049l", 8);
	write(1, "\x1b[?25h", 6);
}

unsigned char& cc8::Display::operator[](unsigned short position)
{
	return m_memory[position];
}

unsigned char cc8::Display::operator[](unsigned short position) const
{
	return m_memory[position];
}

void cc8::Display::show() const
{
	write(1, "\x1b[1;1H", 6);
	for (unsigned char y = 0; y < 32; ++y) {
		for (unsigned char x = 0; x < 8; ++x) {
			for (unsigned char s = 0; s < 8; ++s) {
				if ((m_memory[(y * 8) + x] >> (7 - s)) & 1)
					write(1, "\x1b[107m ", 7);
				else
					write(1, "\x1b[40m ", 6);
			}
		}
		write(1, "\r\n", 2);
	}
	write(1, "\x1b[m\x1b[J", 6);
}

void cc8::Display::clear()
{
	m_memory.fill(0);
}
