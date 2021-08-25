#pragma once

#include <array>

namespace cc8
{
	class Display
	{
		std::array<unsigned char, 0x0100> m_memory;

		public:
			Display();
			~Display();

			unsigned char& operator[](unsigned short position);
			unsigned char operator[](unsigned short position) const;

			void show() const;
			void clear();
	};
}
