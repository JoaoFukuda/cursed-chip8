#pragma once

#include "display.hpp"

#include <string>

namespace cc8 {
	class Memory {
		int m_fd;
		unsigned char* m_memory = nullptr;

		Display m_display;

		public:
			Memory() = delete;
			Memory(const std::string& filename);
			~Memory();

			unsigned char& operator[](unsigned short position);
			unsigned char operator[](unsigned short position) const;

			Display& get_display();
			unsigned short get_instruction(unsigned short) const;
	};
}
