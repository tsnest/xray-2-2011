////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "console_wrapper.h"
#include <xray/engine/console.h>
#include <xray/input/keyboard.h>

using xray::input::enum_keyboard;
namespace xray{
namespace editor{

input::enum_keyboard scan_to_dinput(System::Windows::Forms::KeyEventArgs^ k );

console_wrapper::console_wrapper( )
:m_console	( NULL )
{}

void console_wrapper::on_activate( editor_base::input_keys_holder^ key_holder )
{
	m_active_key_holder					= key_holder;
	key_holder->subscribe_input_handler	( this, true );
	m_console->on_activate				( );
}

void console_wrapper::on_deactivate( editor_base::input_keys_holder^ key_holder )
{
	key_holder->subscribe_input_handler	( this, false );
	m_console->on_deactivate			( );
	m_active_key_holder					= nullptr;
}

bool console_wrapper::get_active( )
{
	return m_console ? m_console->get_active() : false;
}

void console_wrapper::tick( )
{
	m_console->tick();
}

bool console_wrapper::on_key_up( System::Windows::Forms::KeyEventArgs^ k )
{
	input::enum_keyboard kb = scan_to_dinput(k);

	return m_console->on_keyboard_action( NULL, kb, input::kb_key_up);
}

bool console_wrapper::on_key_down( System::Windows::Forms::KeyEventArgs^ k )
{
	input::enum_keyboard kb = scan_to_dinput(k);
	if(kb==xray::input::key_escape || kb==xray::input::key_grave)
	{
		on_deactivate	(m_active_key_holder);
		return			true;
	}
	return m_console->on_keyboard_action( NULL, kb, input::kb_key_down);
}

} // namespace editor

namespace input{
int vk_remap[0xFF]={
	0,				//							0x00
	0,		// VK_LBUTTON        		0x01
	0,		// VK_RBUTTON        		0x02
	0,				// VK_CANCEL         		0x03
	0,		// VK_MBUTTON        		0x04
	0,				// VK_XBUTTON1       		0x05
	0,				// VK_XBUTTON2				0x06
	0,				//							0x07
	key_back,		// VK_BACK           		0x08
	key_tab,		// VK_TAB            		0x09
	0,				//							0x0A
	0,				//							0x0B
	0,				// VK_CLEAR          		0x0C
	key_return,		// VK_RETURN         		0x0D
	0,				//							0x0E
	0,				//							0x0F
	key_lshift,				// VK_SHIFT          		0x10
	key_lcontrol,				// VK_CONTROL        		0x11
	0,				// VK_MENU           		0x12
	key_pause,		// VK_PAUSE          		0x13
	key_capital,		// VK_CAPITAL        		0x14
	key_kana,		// VK_KANA           		0x15
	0,				//			           		0x16
	0,				// VK_JUNJA          		0x17
	0,				// VK_FINAL          		0x18
	key_kanji,		// VK_HANJA          		0x19
	0,				//			          		0x1A
	key_escape,		// VK_ESCAPE         		0x1B
	key_convert,		// VK_CONVERT        		0x1C
	key_noconvert,	// VK_NONCONVERT     		0x1D
	0,				// VK_ACCEPT         		0x1E
	0,				// VK_MODECHANGE     		0x1F
	key_space,		// VK_SPACE          		0x20
	key_prior,		// VK_PRIOR          		0x21
	key_next,		// VK_NEXT           		0x22
	key_end,			// VK_END            		0x23
	key_home,		// VK_HOME           		0x24
	key_left,		// VK_LEFT           		0x25
	key_up,			// VK_UP             		0x26
	key_right,		// VK_RIGHT          		0x27
	key_down,		// VK_DOWN           		0x28
	0,				// VK_SELECT         		0x29
	0,				// VK_PRINT          		0x2A
	0,				// VK_EXECUTE        		0x2B
	key_sysrq,		// VK_SNAPSHOT       		0x2C
	key_insert,		// VK_INSERT         		0x2D
	key_delete,		// VK_DELETE         		0x2E
	0,				// VK_HELP           		0x2F
	key_0,			// VK_0						0x30
	key_1,			// VK_1						0x31
	key_2,			// VK_2						0x32
	key_3,			// VK_3						0x33
	key_4,			// VK_4						0x34
	key_5,			// VK_5						0x35
	key_6,			// VK_6						0x36
	key_7,			// VK_7						0x37
	key_8,			// VK_8						0x38
	key_9,			// VK_9						0x39
	0,				//							0x3A
	0,				//							0x3B
	0,				//							0x3C
	0,				//							0x3D
	0,				//							0x3E
	0,				//							0x3F
	0,				//							0x40
	key_a,			// VK_A						0x41
	key_b,			// VK_B						0x42
	key_c,			// VK_C						0x43
	key_d,			// VK_D						0x44
	key_e,			// VK_E						0x45
	key_f,			// VK_F						0x46
	key_g,			// VK_G						0x47
	key_h,			// VK_H						0x48
	key_i,			// VK_I						0x49
	key_j,			// VK_J						0x4A
	key_k,			// VK_K						0x4B
	key_l,			// VK_L						0x4C
	key_m,			// VK_M						0x4D
	key_n,			// VK_N						0x4E
	key_o,			// VK_O						0x4F
	key_p,			// VK_P						0x50
	key_q,			// VK_Q						0x51
	key_r,			// VK_R						0x52
	key_s,			// VK_S						0x53
	key_t,			// VK_T						0x54
	key_u,			// VK_U						0x55
	key_v,			// VK_V						0x56
	key_w,			// VK_W						0x57
	key_x,			// VK_X						0x58
	key_y,			// VK_Y						0x59
	key_z,			// VK_Z						0x5A
	key_lwin,		// VK_LWIN           		0x5B
	key_rwin,		// VK_RWIN           		0x5C
	key_apps,		// VK_APPS           		0x5D
	0,				//							0x5E
	key_sleep,		// VK_SLEEP          		0x5F
	key_numpad0,		// VK_NUMPAD0        		0x60
	key_numpad1,		// VK_NUMPAD1        		0x61
	key_numpad2,		// VK_NUMPAD2        		0x62
	key_numpad3,		// VK_NUMPAD3        		0x63
	key_numpad4,		// VK_NUMPAD4        		0x64
	key_numpad5,		// VK_NUMPAD5        		0x65
	key_numpad6,		// VK_NUMPAD6        		0x66
	key_numpad7,		// VK_NUMPAD7        		0x67
	key_numpad8,		// VK_NUMPAD8        		0x68
	key_numpad9,		// VK_NUMPAD9        		0x69
	key_multiply,	// VK_MULTIPLY       		0x6A
	key_add,			// VK_ADD            		0x6B
	0,				// VK_SEPARATOR      		0x6C
	key_subtract,	// VK_SUBTRACT       		0x6D
	key_decimal,		// VK_DECIMAL        		0x6E
	key_divide,		// VK_DIVIDE         		0x6F
	key_f1,			// VK_F1             		0x70
	key_f2,			// VK_F2             		0x71
	key_f3,			// VK_F3             		0x72
	key_f4,			// VK_F4             		0x73
	key_f5,			// VK_F5             		0x74
	key_f6,			// VK_F6             		0x75
	key_f7,			// VK_F7             		0x76
	key_f8,			// VK_F8             		0x77
	key_f9,			// VK_F9             		0x78
	key_f10,			// VK_F10            		0x79
	key_f11,			// VK_F11            		0x7A
	key_f12,			// VK_F12            		0x7B
	key_f13,			// VK_F13            		0x7C
	key_f14,			// VK_F14            		0x7D
	key_f15,			// VK_F15            		0x7E
	0,				// VK_F16            		0x7F
	0,				// VK_F17            		0x80
	0,				// VK_F18            		0x81
	0,				// VK_F19            		0x82
	0,				// VK_F20            		0x83
	0,				// VK_F21            		0x84
	0,				// VK_F22            		0x85
	0,				// VK_F23            		0x86
	0,				// VK_F24            		0x87
	0,				// 							0x88
	0,				// 							0x89
	0,				// 							0x8A
	0,				// 							0x8B
	0,				// 							0x8C
	0,				// 							0x8D
	0,				// 							0x8E
	0,				// 							0x8F
	key_numlock,		// VK_NUMLOCK        		0x90
	key_scroll,		// VK_SCROLL         		0x91
	key_numpadequals,// VK_OEM_NEC_EQUAL  		0x92   // '=' key on numpad
	0,				// VK_OEM_FJ_MASSHOU 		0x93   // 'Unregister word' key
	0,				// VK_OEM_FJ_TOUROKU 		0x94   // 'Register word' key
	0,				// VK_OEM_FJ_LOYA    		0x95   // 'Left OYAYUBI' key
	0,				// VK_OEM_FJ_ROYA    		0x96   // 'Right OYAYUBI' key
	0,				// 							0x97
	0,				// 							0x98
	0,				// 							0x99
	0,				// 							0x9A
	0,				// 							0x9B
	0,				// 							0x9C
	0,				// 							0x9D
	0,				// 							0x9E
	0,				// 							0x9F
	key_lshift,		// VK_LSHIFT         		0xA0
	key_rshift,		// VK_RSHIFT         		0xA1
	key_lcontrol,	// VK_LCONTROL       		0xA2
	key_rcontrol,	// VK_RCONTROL       		0xA3
	key_lmenu,		// VK_LMENU          		0xA4
	key_rmenu,		// VK_RMENU          		0xA5
	key_webback,		// VK_BROWSER_BACK        	0xA6
	key_webforward,	// VK_BROWSER_FORWARD     	0xA7
	key_webrefresh,	// VK_BROWSER_REFRESH     	0xA8
	key_webstop,		// VK_BROWSER_STOP        	0xA9
	key_websearch,	// VK_BROWSER_SEARCH      	0xAA
	key_webfavorites,// VK_BROWSER_FAVORITES   	0xAB
	key_webhome,		// VK_BROWSER_HOME        	0xAC
	0,				// VK_VOLUME_MUTE         	0xAD
	key_volumedown,	// VK_VOLUME_DOWN         	0xAE
	key_volumeup,	// VK_VOLUME_UP           	0xAF
	key_nexttrack,	// VK_MEDIA_NEXT_TRACK    	0xB0
	key_prevtrack,	// VK_MEDIA_PREV_TRACK    	0xB1
	key_mediastop,	// VK_MEDIA_STOP          	0xB2
	key_playpause,	// VK_MEDIA_PLAY_PAUSE    	0xB3
	key_mail,		// VK_LAUNCH_MAIL         	0xB4
	key_mediaselect,	// VK_LAUNCH_MEDIA_SELECT 	0xB5
	0,				// VK_LAUNCH_APP1         	0xB6
	0,				// VK_LAUNCH_APP2         	0xB7
	0,				// 							0xB8
	0,				// 							0xB9
	key_semicolon,	// VK_OEM_1          		0xBA   // ';:' for US
	key_add,			// VK_OEM_PLUS       		0xBB   // '+' any country
	key_comma,		// VK_OEM_COMMA      		0xBC   // ',' any country
	key_minus,		// VK_OEM_MINUS      		0xBD   // '-' any country
	key_period,		// VK_OEM_PERIOD     		0xBE   // '.' any country
	key_slash,		// VK_OEM_2          		0xBF   // '/?' for US
	key_grave,		// VK_OEM_3          		0xC0   // '`~' for US
	0,				// 							0xC1
	0,				// 							0xC2
	0,				// 							0xC3
	0,				// 							0xC4
	0,				// 							0xC5
	0,				// 							0xC6
	0,				// 							0xC7
	0,				// 							0xC8
	0,				// 							0xC9
	0,				// 							0xCA
	0,				// 							0xCB
	0,				// 							0xCD
	0,				// 							0xCE
	0,				// 							0xCF
	0,				// 							0xD0
	0,				// 							0xD1
	0,				// 							0xD2
	0,				// 							0xD3
	0,				// 							0xD4
	0,				// 							0xD5
	0,				// 							0xD6
	0,				// 							0xD7
	0,				// 							0xD8
	0,				// 							0xD9
	0,				// 							0xDA
	key_lbracket,	// VK_OEM_4          		0xDB  //  '[{' for US
	key_backslash,	// VK_OEM_5          		0xDC  //  '\|' for US
	key_rbracket,	// VK_OEM_6          		0xDD  //  ']}' for US
	key_apostrophe,	// VK_OEM_7          		0xDE  //  ''"' for US
	0,				// VK_OEM_8          		0xDF
	0,				//							0xE0
	key_ax,			// VK_OEM_AX         		0xE1  //  'AX' key on Japanese AX kbd
	key_oem_102,		// VK_OEM_102        		0xE2  //  "<>" or "\|" on RT 102-key kbd.
	0,				// VK_ICO_HELP       		0xE3  //  Help key on ICO
	0,				// VK_ICO_00         		0xE4  //  00 key on ICO
	0,				// VK_PROCESSKEY     		0xE5
	0,				// VK_ICO_CLEAR      		0xE6
	0,				// VK_PACKET         		0xE7
	0,				//							0xE8
	0,				// VK_OEM_RESET      		0xE9
	0,				// VK_OEM_JUMP       		0xEA
	0,				// VK_OEM_PA1        		0xEB
	0,				// VK_OEM_PA2        		0xEC
	0,				// VK_OEM_PA3        		0xED
	0,				// VK_OEM_WSCTRL     		0xEE
	0,				// VK_OEM_CUSEL      		0xEF
	0,				// VK_OEM_ATTN       		0xF0
	0,				// VK_OEM_FINISH     		0xF1
	0,				// VK_OEM_COPY       		0xF2
	0,				// VK_OEM_AUTO       		0xF3
	0,				// VK_OEM_ENLW       		0xF4
	0,				// VK_OEM_BACKTAB    		0xF5
	0,				// VK_ATTN           		0xF6
	0,				// VK_CRSEL          		0xF7
	0,				// VK_EXSEL          		0xF8
	0,				// VK_EREOF          		0xF9
	0,				// VK_PLAY           		0xFA
	0,				// VK_ZOOM           		0xFB
	0,				// VK_NONAME         		0xFC
	0,				// VK_PA1            		0xFD
	0,				// VK_OEM_CLEAR      		0xFE
};
}// namespace input

namespace editor{
input::enum_keyboard scan_to_dinput(System::Windows::Forms::KeyEventArgs^ k)
{

	if(k->KeyValue < 0xff )
		return (input::enum_keyboard)input::vk_remap[k->KeyValue];
	else
		return input::key_escape;
}

} // namespace editor
} // namespace xray
