#include "chip.hpp"

#include <stdexcept>

void cc8::Chip::draw(unsigned short x, unsigned short y, unsigned char len)
{
	for (unsigned char i = 0; i < len; ++i) {
		m_memory[0x0100 + (y * 8) + (x / 8) + (i * 8)] ^=
		    m_memory[m_address + i] >> (x % 8);
		m_memory[0x0100 + (y * 8) + (x / 8 + 1) + (i * 8)] ^=
		    m_memory[m_address + i] << (8 - x % 8);
	}
}

void cc8::Chip::draw_char(unsigned short x, unsigned short y, unsigned char len,
                          unsigned char c)
{
	for (unsigned char i = 0; i < len; ++i) {
		m_memory[0x0100 + (y * 8) + (x / 8) + (i * 8)] ^=
		    m_memory[0x0800 + (c * 0x0010) + i] >> (x % 8);
		m_memory[0x0100 + (y * 8) + (x / 8 + 1) + (i * 8)] ^=
		    m_memory[0x0800 + (c * 0x0010) + i] << (8 - x % 8);
	}
}

void cc8::Chip::program()
{
	while (m_memory.get_instruction(m_program_counter) != 0xF000) {
		auto opcode = m_memory.get_instruction(m_program_counter);
		auto most_significant_hw = (opcode >> 12) & 0x000F;
		if (opcode == 0x00E0) {
			for (unsigned short m = 0x0100; m < 0x0200; ++m) {
				m_memory[m] = 0x00;
			}
		}
		else if (opcode == 0x00EE) {
			if (m_stack_pointer == 0)
				throw std::runtime_error("Returning with empty stack");
			m_stack_pointer -= 2;
			m_program_counter = (m_memory[m_stack_pointer] << 4) +
			                    (m_memory[m_stack_pointer + 1]) - 2;
		}
		else if (most_significant_hw == 1) {
			m_program_counter = (opcode & 0x0FFF) - 2;
		}
		else if (most_significant_hw == 2) {
			if (m_stack_pointer >= 0x003E)
				throw std::runtime_error("Calling with full stack");
			m_memory[m_stack_pointer] = (m_program_counter >> 4) & 0x00FF;
			m_memory[m_stack_pointer + 1] = m_program_counter & 0x00FF;
			m_stack_pointer += 2;
			m_program_counter = (opcode & 0x0FFF) - 2;
		}
		else if (most_significant_hw == 3) {
			if (m_registers[(opcode >> 8) & 0x000F] == (opcode & 0x00FF)) {
				m_program_counter += 2;
			}
		}
		else if (most_significant_hw == 4) {
			if (m_registers[(opcode >> 8) & 0x000F] != (opcode & 0x00FF)) {
				m_program_counter += 2;
			}
		}
		else if (most_significant_hw == 5 && (opcode & 0x000F) == 0) {
			if (m_registers[(opcode >> 8) & 0x000F] ==
			    m_registers[(opcode >> 4) & 0x000F]) {
				m_program_counter += 2;
			}
		}
		else if (most_significant_hw == 6) {
			m_registers[(opcode >> 8) & 0x000F] = opcode & 0x00FF;
		}
		else if (most_significant_hw == 7) {
			m_registers[(opcode >> 8) & 0x000F] += opcode & 0x00FF;
		}
		else if (most_significant_hw == 8) {
			unsigned char x = (opcode >> 8) & 0x000F, y = (opcode >> 4) & 0x000F;
			switch (opcode & 0x000F) {
				case 0:
					m_registers[x] = m_registers[y];
					break;
				case 1:
					m_registers[x] |= m_registers[y];
					break;
				case 2:
					m_registers[x] &= m_registers[y];
					break;
				case 3:
					m_registers[x] ^= m_registers[y];
					break;
				case 4:
					m_registers[0xF] =
					    (m_registers[x] + m_registers[y] < m_registers[x]) ? 1 : 0;
					m_registers[x] += m_registers[y];
					break;
				case 5:
					m_registers[0xF] =
					    (m_registers[x] - m_registers[y] > m_registers[x]) ? 1 : 0;
					m_registers[x] -= m_registers[y];
					break;
				case 6:
					m_registers[0xF] = m_registers[y] & 1;
					m_registers[x] = m_registers[y] >> 1;
					break;
				case 7:
					m_registers[0xF] =
					    (m_registers[y] - m_registers[x] > m_registers[x]) ? 1 : 0;
					m_registers[x] = m_registers[y] - m_registers[x];
					break;
				case 0xE:
					m_registers[0xF] = (m_registers[y] >> 7) & 1;
					m_registers[x] = m_registers[y] << 1;
					break;
				default:
					throw std::runtime_error("No known opcode with this value");
			}
		}
		else if (most_significant_hw == 9 && (opcode & 0x000F) == 0) {
			if (m_registers[(opcode >> 8) & 0x000F] !=
			    m_registers[(opcode >> 4) & 0x000F]) {
				m_program_counter += 2;
			}
		}
		else if (most_significant_hw == 0xA) {
			m_address = (opcode & 0x0FFF);
		}
		else if (most_significant_hw == 0xB) {
			m_program_counter = m_registers[0] + (opcode & 0x0FFF) - 2;
		}
		else if (most_significant_hw == 0xC) {
			// m_registers[(opcode >> 8) & 0x000F] = dist(rd) & (opcode & 0x00FF);
		}
		else if (most_significant_hw == 0xD) {
			unsigned char x = m_registers[(opcode >> 8) & 0x000F],
			              y = m_registers[(opcode >> 4) & 0x000F],
			              len = opcode & 0x000F;
			draw(x, y, len);
		}
		else if (most_significant_hw == 0xE) {
			// HALP
		}
		else if (most_significant_hw == 0xF) {
			switch (opcode & 0x00FF) {
				case 0x07:
					break;
				case 0x0A:
					m_registers[(opcode >> 8) & 0x000F] = std::getchar();
					break;
				case 0x15:
					break;
				case 0x17:
					break;
				case 0x18:
					break;
				case 0x1E:
					m_address += m_registers[(opcode >> 8) & 0x000F] - 2;
					break;
				case 0x29: {
					unsigned char x = m_registers[(opcode >> 8) & 0x000F];
					m_address = 0x0800 + ((x << 4) & 0x00F0);
				} break;
				case 0x33: {
					unsigned char x = m_registers[(opcode >> 8) & 0x000F];
					m_memory[m_address] = x % 10;
					m_memory[m_address + 1] = (x / 10) % 10;
					m_memory[m_address + 2] = (x / 100) % 10;
				} break;
				case 0x55:
					for (unsigned char i = 0; i <= ((opcode >> 8) & 0x000F); ++i) {
						m_memory[m_address + i + 1] = m_registers[i];
					}
					break;
				case 0x65:
					for (unsigned char i = 0; i <= ((opcode >> 8) & 0x000F); ++i) {
						m_registers[((opcode >> 8) & 0x000F) - i] =
						    m_memory[m_address + i];
					}
					break;
				default:
					throw std::runtime_error("No known opcode with this value");
			}
		}
		else if (opcode != 0x0000) {
			throw std::runtime_error("No known opcode with this value");
		}
		m_program_counter += 2;
	}
	m_mode = Mode::Command;
}
