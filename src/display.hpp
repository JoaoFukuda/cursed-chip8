#pragma once

#include <array>
#include <thread>

namespace cc8 {
	class Display {
		bool quit = false;
		std::array<unsigned char, 0x0100> m_memory;
		std::thread m_update_thread;

		void show() const;
		void update_loop() const;

	 public:
		Display();
		~Display();

		unsigned char& operator[](unsigned short position);
		unsigned char operator[](unsigned short position) const;
	};
}
