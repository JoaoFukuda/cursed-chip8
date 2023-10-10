#include "chip.hpp"

#include <stdexcept>

inline unsigned char char_to_hex(char c)
{
	switch (c) {
		case '0':
			return 0x0;
		case '1':
			return 0x1;
		case '2':
			return 0x2;
		case '3':
			return 0x3;
		case '4':
			return 0x4;
		case '5':
			return 0x5;
		case '6':
			return 0x6;
		case '7':
			return 0x7;
		case '8':
			return 0x8;
		case '9':
			return 0x9;
		case 'a':
		case 'A':
			return 0xA;
		case 'b':
		case 'B':
			return 0xB;
		case 'c':
		case 'C':
			return 0xC;
		case 'd':
		case 'D':
			return 0xD;
		case 'e':
		case 'E':
			return 0xE;
		case 'f':
		case 'F':
			return 0xF;
		default:
			throw std::runtime_error("Input not defined");
	}
}

void cc8::Chip::draw_info(bool draw_value)
{
	for (unsigned char y = 23; y < 32; ++y) {
		for (unsigned char x = 0; x < 8; ++x) {
			m_memory[0x0100 + (y * 8) + x] = 0xFF;
		}
	}

	for (unsigned char i = 0; i < 4; ++i) {
		draw_char(0x0A + (i * 0x05), 25, 5, (m_address >> (3 - i) * 4) & 0x000F);
	}

	if (draw_value) {
		for (unsigned char i = 0; i < 2; ++i) {
			draw_char(0x25 + (i * 0x05), 25, 5,
			          (m_memory[m_address] >> (1 - i) * 4) & 0x000F);
		}
	}
}

void cc8::Chip::command()
{
	draw_info();
	char input = std::getchar();
	switch (input) {
		case 0x1b:
			if (std::getchar() == 'O') {
				switch (std::getchar()) {
					case 'P': // F1 button
						switch (std::getchar()) {
							case '0':
								m_mode = Mode::Edit;
								break;
							case '1':
								// SAVE TO EEPROM
								break;
							case '2':
								// LOAD FROM EEPROM
								break;
							case '3':
								m_stack_pointer = 0x0000;
								m_program_counter = 0x0200;
								m_mode = Mode::Program;
								break;
						}
						break;
					case 'Q': // F2 button
						m_mode = Mode::Quit;
						break;
				}
			}
			else
				throw std::runtime_error("Input not defined");
			break;
		default:
			m_address = (char_to_hex(input) << 12) + (m_address & 0x0FFF);
			for (unsigned char i = 1; i < 4; ++i) {
				draw_info();
				m_address = (char_to_hex(std::getchar()) << (12 - i * 4)) +
				            (m_address & ~(0xF << (12 - i * 4)));
			}
			break;
	}
}

void cc8::Chip::edit()
{
	draw_info(true);
	char input = std::getchar();
	switch (input) {
		case 0x1b:
			if (std::getchar() == 'O') {
				switch (std::getchar()) {
					case 'P':
						++m_address;
						break;
					case 'Q':
						m_mode = Mode::Command;
						break;
				}
			}
			else
				throw std::runtime_error("Input not defined");
			break;
		default:
			m_memory[m_address] =
			    (char_to_hex(input) << 4) + (m_memory[m_address] & 0x0F);
			draw_info(true);
			m_memory[m_address] =
			    char_to_hex(std::getchar()) + (m_memory[m_address] & 0xF0);
			++m_address;
			break;
	}
}

cc8::Chip::Chip(const std::string& filename) : m_memory(filename) { }

cc8::Chip::~Chip() { }

void cc8::Chip::run()
{
	bool quit = false;
	while (!quit) {
		try {
			switch (m_mode) {
				case Mode::Command:
					command();
					break;

				case Mode::Edit:
					edit();
					break;

				case Mode::Program:
					program();
					break;

				case Mode::Quit:
					quit = true;
					break;
			}
		}
		catch (std::runtime_error& e) {
			// runtime_error s are probably input errors, so ignore them
		}
	}
}
