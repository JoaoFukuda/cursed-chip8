#pragma once

#include "memory.hpp"

#include <array>
#include <random>
#include <string>

namespace cc8 {
	class Chip {
		enum class Mode
		{
			Command,
			Edit,
			Program,
			Quit
		};

		Mode m_mode = Mode::Command;

		unsigned short m_address = 0x4457;
		unsigned short m_stack_pointer;
		unsigned short m_program_counter;
		std::array<unsigned char, 16> m_registers;

		Memory m_memory;

		void command();
		void edit();
		void program();

		void draw(unsigned short x, unsigned short y, unsigned char len);
		void draw_char(unsigned short x, unsigned short y, unsigned char len,
		               unsigned char c);
		void draw_info(bool draw_value = false);

	 public:
		Chip(const std::string& filename);
		~Chip();

		void run();
	};
}
