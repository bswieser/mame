// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/******************************************************************************

    Psion Series 3a/3c/3mx

    TODO:
    - battery backed RAM
    - sound devices
    - serial ports
    - fix RAM detection for 3mx

******************************************************************************/

#include "emu.h"
#include "machine/psion_asic9.h"
#include "machine/psion_ssd.h"
#include "machine/ram.h"
#include "sound/spkrdev.h"
#include "bus/psion/honda/slot.h"
#include "bus/psion/sibo/slot.h"

#include "emupal.h"
#include "screen.h"
#include "softlist_dev.h"
#include "speaker.h"
#include "utf8.h"


namespace {

class psion3a_state : public driver_device
{
public:
	psion3a_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_asic9(*this, "asic9")
		, m_ram(*this, "ram")
		, m_palette(*this, "palette")
		, m_keyboard(*this, "COL%u", 0U)
		, m_speaker(*this, "speaker")
		, m_ssd(*this, "ssd%u", 1U)
		, m_sibo(*this, "sibo")
		, m_honda(*this, "honda")
	{ }

	void psion_asic9(machine_config &config);
	void psion3a(machine_config &config);
	void psion3a2(machine_config &config);
	void psion3c(machine_config &config);
	void psion3mx(machine_config &config);

	DECLARE_INPUT_CHANGED_MEMBER(wakeup);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	required_device<psion_asic9_device> m_asic9;
	required_device<ram_device> m_ram;
	required_device<palette_device> m_palette;
	required_ioport_array<8> m_keyboard;
	required_device<speaker_sound_device> m_speaker;
	required_device_array<psion_ssd_device, 2> m_ssd;
	optional_device<psion_sibo_slot_device> m_sibo;
	optional_device<psion_honda_slot_device> m_honda;

	void palette_init(palette_device &palette);

	uint16_t kbd_r();

	uint8_t m_key_col = 0;
};


void psion3a_state::machine_start()
{
}

void psion3a_state::machine_reset()
{
}


static INPUT_PORTS_START( psion3a )
	PORT_START("COL0")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER)      PORT_CHAR(13)                                   PORT_NAME("Enter")
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_RIGHT)      PORT_CHAR(UCHAR_MAMEKEY(RIGHT))                 PORT_NAME(UTF8_RIGHT" End")
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_TAB)        PORT_CHAR(0x09)                                 PORT_NAME("Tab")
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)          PORT_CHAR('y')  PORT_CHAR('Y')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT)       PORT_CHAR(UCHAR_MAMEKEY(LEFT))                  PORT_NAME(UTF8_LEFT" Home")
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN)       PORT_CHAR(UCHAR_MAMEKEY(DOWN))                  PORT_NAME(UTF8_DOWN" Pg Dn")
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_N)          PORT_CHAR('n')  PORT_CHAR('N')
	PORT_BIT(0x080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LALT)       PORT_CHAR(UCHAR_SHIFT_2)                        PORT_NAME("Psion")
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F8)         PORT_CHAR(UCHAR_MAMEKEY(F8))                    PORT_NAME("Sheet")           PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F5)         PORT_CHAR(UCHAR_MAMEKEY(F5))                    PORT_NAME("Time")            PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F2)         PORT_CHAR(UCHAR_MAMEKEY(F2))                    PORT_NAME("Data")            PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)

	PORT_START("COL1")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)      PORT_CHAR('/')  PORT_CHAR(';')
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)      PORT_CHAR('-')  PORT_CHAR('_')
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)     PORT_CHAR('+')  PORT_CHAR('=')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)          PORT_CHAR('0')  PORT_CHAR(')')  PORT_CHAR(']')
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_P)          PORT_CHAR('p')  PORT_CHAR('P')
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)      PORT_CHAR('*')  PORT_CHAR(':')
	PORT_BIT(0x080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LSHIFT)     PORT_CHAR(UCHAR_SHIFT_1)                        PORT_NAME("Shift (L)")
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F7)         PORT_CHAR(UCHAR_MAMEKEY(F7))                    PORT_NAME("Calc")            PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F4)         PORT_CHAR(UCHAR_MAMEKEY(F4))                    PORT_NAME("Agenda")          PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F1)         PORT_CHAR(UCHAR_MAMEKEY(F1))                    PORT_NAME("System")          PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)

	PORT_START("COL2")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(0x08)                                 PORT_NAME("Delete")
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_K)          PORT_CHAR('k')  PORT_CHAR('K')
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_I)          PORT_CHAR('i')  PORT_CHAR('I')
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_8)          PORT_CHAR('8')  PORT_CHAR('?')  PORT_CHAR('}')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9)          PORT_CHAR('9')  PORT_CHAR('(')  PORT_CHAR('[')
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_O)          PORT_CHAR('o')  PORT_CHAR('O')
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_L)          PORT_CHAR('l')  PORT_CHAR('L')
	PORT_BIT(0x080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LCONTROL)   PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))              PORT_NAME("Control")
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F6)         PORT_CHAR(UCHAR_MAMEKEY(F6))                    PORT_NAME("World")           PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F3)         PORT_CHAR(UCHAR_MAMEKEY(F3))                    PORT_NAME("Word")            PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)

	PORT_START("COL3")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)      PORT_CHAR(',')  PORT_CHAR('<')
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F10)        PORT_CHAR(UCHAR_MAMEKEY(F10))                   PORT_NAME("Help Dial")
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_M)          PORT_CHAR('m')  PORT_CHAR('M')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_J)          PORT_CHAR('j')  PORT_CHAR('J')
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_U)          PORT_CHAR('u')  PORT_CHAR('U')
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_7)          PORT_CHAR('7')  PORT_CHAR('&')  PORT_CHAR('{')
	PORT_BIT(0x080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_RSHIFT)     PORT_CHAR(UCHAR_SHIFT_1)                        PORT_NAME("Shift (R)")

	PORT_START("COL4")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE)      PORT_CHAR(' ')
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_R)          PORT_CHAR('r')  PORT_CHAR('R')
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4)          PORT_CHAR('4')  PORT_CHAR('$')  PORT_CHAR('~')
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5)          PORT_CHAR('5')  PORT_CHAR('%')  PORT_CHAR('\'')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_T)          PORT_CHAR('t')  PORT_CHAR('T')
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_G)          PORT_CHAR('g')  PORT_CHAR('G')
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_B)          PORT_CHAR('b')  PORT_CHAR('B')
	PORT_BIT(0x080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_CAPSLOCK)   PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))              PORT_NAME("\xe2\x97\x86 Caps")

	PORT_START("COL5")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F)          PORT_CHAR('f')  PORT_CHAR('F')
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_V)          PORT_CHAR('v')  PORT_CHAR('V')
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_C)          PORT_CHAR('c')  PORT_CHAR('C')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_D)          PORT_CHAR('d')  PORT_CHAR('D')
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_E)          PORT_CHAR('e')  PORT_CHAR('E')
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)          PORT_CHAR('3')  PORT_CHAR(0xa3) PORT_CHAR('\\')
	PORT_BIT(0x080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F11)        PORT_CHAR(UCHAR_MAMEKEY(F11))                   PORT_NAME("Menu")

	PORT_START("COL6")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q)          PORT_CHAR('q')  PORT_CHAR('Q')
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_A)          PORT_CHAR('a')  PORT_CHAR('A')
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z)          PORT_CHAR('z')  PORT_CHAR('Z')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_S)          PORT_CHAR('s')  PORT_CHAR('S')
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_W)          PORT_CHAR('w')  PORT_CHAR('W')
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_X)          PORT_CHAR('x')  PORT_CHAR('X')

	PORT_START("COL7")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)          PORT_CHAR('1')  PORT_CHAR('!')                  PORT_NAME("1 ! Off")
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_2)          PORT_CHAR('2')  PORT_CHAR('"')  PORT_CHAR('#')
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6)          PORT_CHAR('6')  PORT_CHAR('^')  PORT_CHAR('@')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)       PORT_CHAR('.')  PORT_CHAR('>')
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_UP)         PORT_CHAR(UCHAR_MAMEKEY(UP))                    PORT_NAME(UTF8_UP" Pg Up")
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_H)          PORT_CHAR('h')  PORT_CHAR('H')
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC)        PORT_CHAR(UCHAR_MAMEKEY(ESC))                   PORT_NAME("Esc On")          PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
INPUT_PORTS_END


static INPUT_PORTS_START( psion3a_de )
	PORT_INCLUDE(psion3a)

	PORT_MODIFY("COL0")
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_RIGHT)      PORT_CHAR(UCHAR_MAMEKEY(RIGHT))                 PORT_NAME(UTF8_RIGHT" Ende")
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)          PORT_CHAR('z')  PORT_CHAR('Z')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT)       PORT_CHAR(UCHAR_MAMEKEY(LEFT))                  PORT_NAME(UTF8_LEFT" Pos1")
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN)       PORT_CHAR(UCHAR_MAMEKEY(DOWN))                  PORT_NAME(UTF8_DOWN" Bild Dn")
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F8)         PORT_CHAR(UCHAR_MAMEKEY(F8))                    PORT_NAME("Tabelle")         PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F5)         PORT_CHAR(UCHAR_MAMEKEY(F5))                    PORT_NAME("Uhr")             PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F2)         PORT_CHAR(UCHAR_MAMEKEY(F2))                    PORT_NAME("Daten")           PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)

	PORT_MODIFY("COL1")
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)      PORT_CHAR('/')  PORT_CHAR(0xdf)
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)     PORT_CHAR('+')  PORT_CHAR('<')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)          PORT_CHAR('0')  PORT_CHAR('=')  PORT_CHAR('}')
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)      PORT_CHAR('*')  PORT_CHAR('>')
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F7)         PORT_CHAR(UCHAR_MAMEKEY(F7))                    PORT_NAME("Rechner")         PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)

	PORT_MODIFY("COL2")
	PORT_BIT(0x001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(0x08)                                 PORT_NAME("Entf")
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_8)          PORT_CHAR('8')  PORT_CHAR('(')  PORT_CHAR('[')
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9)          PORT_CHAR('9')  PORT_CHAR(')')  PORT_CHAR(']')
	PORT_BIT(0x080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LCONTROL)   PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))              PORT_NAME("Strg")
	PORT_BIT(0x200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F6)         PORT_CHAR(UCHAR_MAMEKEY(F6))                    PORT_NAME("Welt")            PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)

	PORT_MODIFY("COL3")
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)      PORT_CHAR(',')  PORT_CHAR(';')
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F10)        PORT_CHAR(UCHAR_MAMEKEY(F10))                   PORT_NAME("Hilfe Wahl")
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_7)          PORT_CHAR('7')  PORT_CHAR('?')  PORT_CHAR('{')

	PORT_MODIFY("COL4")
	PORT_BIT(0x004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4)          PORT_CHAR('4')  PORT_CHAR('$')  PORT_CHAR('@')
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5)          PORT_CHAR('5')  PORT_CHAR('%')  PORT_CHAR('\'')

	PORT_MODIFY("COL5")
	PORT_BIT(0x040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)          PORT_CHAR('3')  PORT_CHAR(0xa7) PORT_CHAR('\\')

	PORT_MODIFY("COL6")
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z)          PORT_CHAR('y')  PORT_CHAR('Y')

	PORT_MODIFY("COL7")
	PORT_BIT(0x002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)          PORT_CHAR('1')  PORT_CHAR('!')                  PORT_NAME("1 ! Aus")
	PORT_BIT(0x008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6)          PORT_CHAR('6')  PORT_CHAR('&')  PORT_CHAR(0xba)
	PORT_BIT(0x010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)       PORT_CHAR('.')  PORT_CHAR(':')
	PORT_BIT(0x020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_UP)         PORT_CHAR(UCHAR_MAMEKEY(UP))                    PORT_NAME(UTF8_UP" Bild Up")
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC)        PORT_CHAR(UCHAR_MAMEKEY(ESC))                   PORT_NAME("Esc Ein")         PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
INPUT_PORTS_END


static INPUT_PORTS_START( psion3c )
	PORT_INCLUDE(psion3a)

	PORT_MODIFY("COL2")
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F9)         PORT_CHAR(UCHAR_MAMEKEY(F9))                    PORT_NAME("Jotter")          PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
INPUT_PORTS_END


//static INPUT_PORTS_START( psion3c_de )
//  PORT_INCLUDE(psion3a_de)
//
//  PORT_MODIFY("COL2")
//  PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F9)         PORT_CHAR(UCHAR_MAMEKEY(F9))                    PORT_NAME("Notiz")           PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
//INPUT_PORTS_END


static INPUT_PORTS_START( pocketbk2 )
	PORT_INCLUDE(psion3a)

	PORT_MODIFY("COL0")
	PORT_BIT(0x080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LALT)       PORT_CHAR(UCHAR_SHIFT_2)                        PORT_NAME("Acorn")
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F8)         PORT_CHAR(UCHAR_MAMEKEY(F8))                    PORT_NAME("Abacus")          PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F5)         PORT_CHAR(UCHAR_MAMEKEY(F5))                    PORT_NAME("Time")            PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F2)         PORT_CHAR(UCHAR_MAMEKEY(F2))                    PORT_NAME("Cards")           PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)

	PORT_MODIFY("COL1")
	PORT_BIT(0x100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F7)         PORT_CHAR(UCHAR_MAMEKEY(F7))                    PORT_NAME("Calc")            PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F4)         PORT_CHAR(UCHAR_MAMEKEY(F4))                    PORT_NAME("Schedule")        PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F1)         PORT_CHAR(UCHAR_MAMEKEY(F1))                    PORT_NAME("Desktop")         PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)

	PORT_MODIFY("COL2")
	PORT_BIT(0x200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F6)         PORT_CHAR(UCHAR_MAMEKEY(F6))                    PORT_NAME("World")           PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
	PORT_BIT(0x400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F3)         PORT_CHAR(UCHAR_MAMEKEY(F3))                    PORT_NAME("Write")           PORT_CHANGED_MEMBER(DEVICE_SELF, psion3a_state, wakeup, 0)
INPUT_PORTS_END


INPUT_CHANGED_MEMBER(psion3a_state::wakeup)
{
	m_asic9->eint0_w(newval);
}


uint16_t psion3a_state::kbd_r()
{
	uint16_t data = 0x00;

	for (int i = 0; i < 8; i++)
	{
		if (BIT(m_key_col, i))
			data |= m_keyboard[i]->read();
	}

	return data;
}


void psion3a_state::palette_init(palette_device &palette)
{
	palette.set_pen_color(0, rgb_t(190, 220, 190));
	palette.set_pen_color(1, rgb_t(130, 130, 110));
	palette.set_pen_color(2, rgb_t(190, 210, 180));
}


void psion3a_state::psion_asic9(machine_config &config)
{
	PSION_ASIC9(config, m_asic9, 7.68_MHz_XTAL); // V30H
	m_asic9->set_ram_rom("ram", "rom");
	m_asic9->port_ab_r().set(FUNC(psion3a_state::kbd_r));
	m_asic9->buz_cb().set(m_speaker, FUNC(speaker_sound_device::level_w));
	//m_asic9->buzvol_cb().set([this](int state) { m_speaker->set_output_gain(ALL_OUTPUTS, state ? 1.0 : 0.25); });
	m_asic9->col_cb().set([this](uint8_t data) { m_key_col = data; });
	m_asic9->data_r<0>().set(m_ssd[1], FUNC(psion_ssd_device::data_r));      // SSD Pack 2
	m_asic9->data_w<0>().set(m_ssd[1], FUNC(psion_ssd_device::data_w));
	m_asic9->data_r<1>().set(m_ssd[0], FUNC(psion_ssd_device::data_r));      // SSD Pack 1
	m_asic9->data_w<1>().set(m_ssd[0], FUNC(psion_ssd_device::data_w));

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_LCD));
	screen.set_size(480, 160);
	screen.set_visarea_full();
	screen.set_refresh_hz(66);
	screen.set_screen_update(m_asic9, FUNC(psion_asic9_device::screen_update));
	screen.set_palette(m_palette);
	PALETTE(config, "palette", FUNC(psion3a_state::palette_init), 3);

	SPEAKER(config, "mono").front_center();
	SPEAKER_SOUND(config, m_speaker).add_route(ALL_OUTPUTS, "mono", 1.00); // Piezo buzzer

	RAM(config, m_ram);

	PSION_SSD(config, m_ssd[0]);
	m_ssd[0]->door_cb().set(m_asic9, FUNC(psion_asic9_device::medchng_w));
	PSION_SSD(config, m_ssd[1]);
	m_ssd[1]->door_cb().set(m_asic9, FUNC(psion_asic9_device::medchng_w));

	SOFTWARE_LIST(config, "ssd_list").set_original("psion_ssd").set_filter("S3,S3A");
}

void psion3a_state::psion3a(machine_config &config)
{
	psion_asic9(config);

	m_ram->set_default_size("512K").set_extra_options("256K");

	// SIBO expansion port
	PSION_SIBO_SLOT(config, m_sibo, psion_sibo_devices, nullptr);
	//m_sibo->int_cb().set(m_asic9, FUNC(psion_asic9_device::eint_w)); // TODO: unknown interrupt line
	m_asic9->data_r<4>().set(m_sibo, FUNC(psion_sibo_slot_device::data_r));
	m_asic9->data_w<4>().set(m_sibo, FUNC(psion_sibo_slot_device::data_w));

	// TODO: M7542
}

void psion3a_state::psion3a2(machine_config &config)
{
	psion3a(config);

	m_ram->set_default_size("2M").set_extra_options("1M");
}

void psion3a_state::psion3c(machine_config &config)
{
	psion_asic9(config);

	m_ram->set_default_size("2M").set_extra_options("1M");

	// TODO: M7702

	// TODO: unknown Temic device, likely provides RS232/Parallel to Honda port

	// Honda expansion port
	PSION_HONDA_SLOT(config, m_honda, psion_honda_devices, nullptr);
	//m_honda->int_cb().set(m_asic9, FUNC(psion_asic9_device::eint_w)); // TODO: unknown interrupt line
	m_asic9->data_r<4>().set(m_honda, FUNC(psion_honda_slot_device::data_r));
	m_asic9->data_w<4>().set(m_honda, FUNC(psion_honda_slot_device::data_w));
}

void psion3a_state::psion3mx(machine_config &config)
{
	psion_asic9(config);

	m_asic9->set_clock(3.6864_MHz_XTAL * 15 / 2); // V30MX

	m_ram->set_default_size("2M").set_extra_options("");

	// TODO: M7702

	// Honda expansion port
	PSION_HONDA_SLOT(config, m_honda, psion_honda_devices, nullptr);
	//m_honda->int_cb().set(m_asic9, FUNC(psion_asic9_device::eint_w)); // TODO: unknown interrupt line
	m_asic9->data_r<4>().set(m_honda, FUNC(psion_honda_slot_device::data_r));
	m_asic9->data_w<4>().set(m_honda, FUNC(psion_honda_slot_device::data_w));
}


ROM_START(psion3a)
	ROM_REGION16_LE(0x100000, "rom", 0)
	ROM_SYSTEM_BIOS(0, "322f", "V3.22F/ENG")
	ROMX_LOAD("s3a_v3.22f_eng.bin", 0x000000, 0x100000, CRC(fafa3820) SHA1(c1a320b43280cfdb74fc1cb1363fca88dd187487), ROM_BIOS(0))
ROM_END

ROM_START(psion3a2)
	ROM_REGION16_LE(0x200000, "rom", 0)
	ROM_SYSTEM_BIOS(0, "340f", "V3.40F/ENG")
	ROMX_LOAD("s3a_v3.40f_eng.bin", 0x000000, 0x200000, CRC(f0adf12c) SHA1(3eb4e7f1fc5611a4d6e65d27d336969ebae94395), ROM_BIOS(0))
ROM_END

ROM_START(psion3a2_de)
	ROM_REGION16_LE(0x200000, "rom", 0)
	ROM_SYSTEM_BIOS(0, "341f", "V3.41F/DEU")
	ROMX_LOAD("s3a_v3.41f_deu.bin", 0x000000, 0x200000, CRC(1f21cb0a) SHA1(fbb9c3356cf8b1d89b8cf50fc12175568c74ce3e), ROM_BIOS(0))
ROM_END

ROM_START(psion3c)
	// Versions advertised: English, German, French, Flemish and Dutch
	ROM_REGION16_LE(0x200000, "rom", 0)
	ROM_SYSTEM_BIOS(0, "520f", "V5.20F/ENG")
	ROMX_LOAD("oak_v5.20f_eng.bin", 0x000000, 0x200000, CRC(d8e672ca) SHA1(23e7570ddbecbfd50953ce6a6b7ead7128814402), ROM_BIOS(0))
ROM_END

ROM_START(psion3mx)
	ROM_REGION16_LE(0x200000, "rom", 0)
	ROM_SYSTEM_BIOS(0, "616f", "V6.16F/ENG")
	ROMX_LOAD("maple_v6.16f_uk.bin", 0x000000, 0x200000, CRC(10011d9d) SHA1(8c657414513ed57ccf6beddc65dca1fe5ab600fb), ROM_BIOS(0))
ROM_END

ROM_START(pocketbk2)
	ROM_REGION16_LE(0x200000, "rom", 0)
	ROM_SYSTEM_BIOS(0, "130f", "V1.30F/ACN")
	ROMX_LOAD("pb2_v1.30f_acn.bin", 0x000000, 0x200000, CRC(d7ba3a50) SHA1(2d29a7ac5ec4d6bf63bd7d93cc3763e0f9763136), ROM_BIOS(0))
ROM_END

} // anonymous namespace


//    YEAR  NAME          PARENT    COMPAT  MACHINE    INPUT        CLASS          INIT         COMPANY             FULLNAME                    FLAGS
COMP( 1993, psion3a,      0,        0,      psion3a,   psion3a,     psion3a_state, empty_init,  "Psion",            "Series 3a",                MACHINE_NOT_WORKING )
COMP( 1994, pocketbk2,    psion3a,  0,      psion3a,   pocketbk2,   psion3a_state, empty_init,  "Acorn Computers",  "Pocket Book II",           MACHINE_NOT_WORKING )
COMP( 1995, psion3a2,     psion3a,  0,      psion3a2,  psion3a,     psion3a_state, empty_init,  "Psion",            "Series 3a (2M)",           MACHINE_NOT_WORKING )
COMP( 1995, psion3a2_de,  psion3a,  0,      psion3a2,  psion3a_de,  psion3a_state, empty_init,  "Psion",            "Series 3a (2M) (German)",  MACHINE_NOT_WORKING )
//COMP( 1996, psion3a2_ru,  psion3a,  0,      psion3a2,  psion3a,     psion3a_state, empty_init,  "Psion",            "Series 3a (2M) (Russian)", MACHINE_NOT_WORKING )
COMP( 1996, psion3c,      0,        0,      psion3c,   psion3c,     psion3a_state, empty_init,  "Psion",            "Series 3c",                MACHINE_NOT_WORKING )
COMP( 1998, psion3mx,     0,        0,      psion3mx,  psion3c,     psion3a_state, empty_init,  "Psion",            "Series 3mx",               MACHINE_NOT_WORKING )
