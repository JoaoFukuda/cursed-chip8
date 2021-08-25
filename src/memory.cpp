#include "memory.hpp"

#include <stdexcept>
#include <random>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

cc8::Memory::Memory(const std::string& filename)
{
	std::random_device rd;
	std::uniform_int_distribution<unsigned char> dist(0x00, 0xFF);

	m_fd = open(filename.c_str(), O_RDONLY);
	if (m_fd == -1) {
		throw std::runtime_error("Could not open file");
	}

	unsigned long m_memory_len = lseek(m_fd, 0, SEEK_END);
	if (m_memory_len != 65536) {
		close(m_fd);
		throw std::runtime_error("Memory file is not 65536 bytes in size");
	}

	m_memory = reinterpret_cast<unsigned char*>(
			mmap(nullptr, m_memory_len, PROT_READ | PROT_WRITE, MAP_PRIVATE,
				m_fd, 0));
	if (m_memory == MAP_FAILED) {
		close(m_fd);
		throw std::runtime_error("Could not map file to memory");
	}

	for (unsigned short i = 0; i < 0x0100; ++i) {
		m_memory[0x0100 + i] = dist(rd);
	}
}

cc8::Memory::~Memory()
{
	munmap(reinterpret_cast<void*>(m_memory), 65536);
	close(m_fd);
}

unsigned char& cc8::Memory::operator[](unsigned short position)
{
	if (position < 0x0100) {
		return m_memory[position];
	}
	else if (position < 0x0200) {
		return m_display[position - 0x0100];
	}
	else {
		return m_memory[position];
	}
}

unsigned char cc8::Memory::operator[](unsigned short position) const
{
	if (position < 0x0100) {
		return m_memory[position];
	}
	else if (position < 0x0200) {
		return m_display[position - 0x0100];
	}
	else {
		return m_memory[position];
	}
}

cc8::Display& cc8::Memory::get_display()
{
	return m_display;
}

unsigned short cc8::Memory::get_instruction(unsigned short position) const
{
	return ((*this)[position] << 8) + (*this)[position + 1];
}
