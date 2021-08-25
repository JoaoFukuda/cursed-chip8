#include "chip.hpp"

void cc8::Chip::draw_info(bool draw_value)
{
	for (unsigned char y = 23; y < 32; ++y) {
		for (unsigned char x = 0; x < 8; ++x) {
			m_memory[0x0100 + (y*8) + x] = 0xFF;
		}
	}

	for (unsigned char i = 0; i < 4; ++i) {
		draw_char(0x0A + (i * 0x05), 25, 5, (m_address >> (3 - i)*4) & 0x000F);
	}

	if (draw_value) {
		for (unsigned char i = 0; i < 2; ++i) {
			draw_char(0x25 + (i * 0x05), 25, 5, (m_memory[m_address] >> (1 - i)*4) & 0x000F);
		}
	}
}

void cc8::Chip::command()
{
	draw_info();
	m_memory.get_display().show();
	switch (std::getchar()) {
		case 'o':
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
		case 't':
			// RESET MONITOR
			break;
		case 'q':
			m_mode = Mode::Quit;
			break;
	}
}

void cc8::Chip::edit()
{
	draw_info(true);
	m_memory.get_display().show();
	switch (std::getchar()) {
		case 'o':
			++m_address;
			break;
		case 't':
			m_mode = Mode::Command;
			break;
	}
}

cc8::Chip::Chip(const std::string& filename)
	: m_memory(filename)
{ }

cc8::Chip::~Chip()
{ }

void cc8::Chip::run()
{
	bool quit = false;
	while (!quit) {
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
}
