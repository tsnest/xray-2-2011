////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INPUT_KEYBOARD_H_INCLUDED
#define XRAY_INPUT_KEYBOARD_H_INCLUDED

namespace xray {
namespace input {

enum enum_keyboard_action{
	kb_key_unknown,
	kb_key_down,
	kb_key_up, 
	kb_key_hold
};

enum enum_keyboard {
	key_escape          = 0x01,
	key_1               = 0x02,
	key_2               = 0x03,
	key_3               = 0x04,
	key_4               = 0x05,
	key_5               = 0x06,
	key_6               = 0x07,
	key_7               = 0x08,
	key_8               = 0x09,
	key_9               = 0x0A,
	key_0               = 0x0B,
	key_minus           = 0x0C,    /* - on main keyboard */
	key_equals          = 0x0D,
	key_back            = 0x0E,    /* backspace */
	key_tab             = 0x0F,
	key_q               = 0x10,
	key_w               = 0x11,
	key_e               = 0x12,
	key_r               = 0x13,
	key_t               = 0x14,
	key_y               = 0x15,
	key_u               = 0x16,
	key_i               = 0x17,
	key_o               = 0x18,
	key_p               = 0x19,
	key_lbracket        = 0x1A,
	key_rbracket        = 0x1B,
	key_return          = 0x1C,    /* Enter on main keyboard */
	key_lcontrol        = 0x1D,
	key_a               = 0x1E,
	key_s               = 0x1F,
	key_d               = 0x20,
	key_f               = 0x21,
	key_g               = 0x22,
	key_h               = 0x23,
	key_j               = 0x24,
	key_k               = 0x25,
	key_l               = 0x26,
	key_semicolon       = 0x27,
	key_apostrophe      = 0x28,
	key_grave           = 0x29,    /* accent grave */
	key_lshift          = 0x2A,
	key_backslash       = 0x2B,
	key_z               = 0x2C,
	key_x               = 0x2D,
	key_c               = 0x2E,
	key_v               = 0x2F,
	key_b               = 0x30,
	key_n               = 0x31,
	key_m               = 0x32,
	key_comma           = 0x33,
	key_period          = 0x34,    /* . on main keyboard */
	key_slash           = 0x35,    /* / on main keyboard */
	key_rshift          = 0x36,
	key_multiply        = 0x37,    /* * on numeric keypad */
	key_lmenu           = 0x38,    /* left Alt */
	key_space           = 0x39,
	key_capital         = 0x3A,
	key_f1              = 0x3B,
	key_f2              = 0x3C,
	key_f3              = 0x3D,
	key_f4              = 0x3E,
	key_f5              = 0x3F,
	key_f6              = 0x40,
	key_f7              = 0x41,
	key_f8              = 0x42,
	key_f9              = 0x43,
	key_f10             = 0x44,
	key_numlock         = 0x45,
	key_scroll          = 0x46,    /* Scroll Lock */
	key_numpad7         = 0x47,
	key_numpad8         = 0x48,
	key_numpad9         = 0x49,
	key_subtract        = 0x4A,    /* - on numeric keypad */
	key_numpad4         = 0x4B,
	key_numpad5         = 0x4C,
	key_numpad6         = 0x4D,
	key_add             = 0x4E,    /* + on numeric keypad */
	key_numpad1         = 0x4F,
	key_numpad2         = 0x50,
	key_numpad3         = 0x51,
	key_numpad0         = 0x52,
	key_decimal         = 0x53,    /* . on numeric keypad */
	key_oem_102         = 0x56,    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	key_f11             = 0x57,
	key_f12             = 0x58,
	key_f13             = 0x64,    /*                     (NEC PC98) */
	key_f14             = 0x65,    /*                     (NEC PC98) */
	key_f15             = 0x66,    /*                     (NEC PC98) */
	key_kana            = 0x70,    /* (Japanese keyboard)            */
	key_abnt_c1         = 0x73,    /* /? on Brazilian keyboard */
	key_convert         = 0x79,    /* (Japanese keyboard)            */
	key_noconvert       = 0x7B,    /* (Japanese keyboard)            */
	key_yen             = 0x7D,    /* (Japanese keyboard)            */
	key_abnt_c2         = 0x7E,    /* Numpad . on Brazilian keyboard */
	key_numpadequals    = 0x8D,    /* = on numeric keypad (NEC PC98) */
	key_prevtrack       = 0x90,    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
	key_at              = 0x91,    /*                     (NEC PC98) */
	key_colon           = 0x92,    /*                     (NEC PC98) */
	key_underline       = 0x93,    /*                     (NEC PC98) */
	key_kanji           = 0x94,    /* (Japanese keyboard)            */
	key_stop            = 0x95,    /*                     (NEC PC98) */
	key_ax              = 0x96,    /*                     (Japan AX) */
	key_unlabeled       = 0x97,    /*                        (J3100) */
	key_nexttrack       = 0x99,    /* Next Track */
	key_numpadenter     = 0x9C,    /* Enter on numeric keypad */
	key_rcontrol        = 0x9D,
	key_mute            = 0xA0,    /* Mute */
	key_calculator      = 0xA1,    /* Calculator */
	key_playpause       = 0xA2,    /* Play / Pause */
	key_mediastop       = 0xA4,    /* Media Stop */
	key_volumedown      = 0xAE,    /* Volume - */
	key_volumeup        = 0xB0,    /* Volume + */
	key_webhome         = 0xB2,    /* Web home */
	key_numpadcomma     = 0xB3,    /* , on numeric keypad (NEC PC98) */
	key_divide          = 0xB5,    /* / on numeric keypad */
	key_sysrq           = 0xB7,
	key_rmenu           = 0xB8,    /* right Alt */
	key_pause           = 0xC5,    /* Pause */
	key_home            = 0xC7,    /* Home on arrow keypad */
	key_up              = 0xC8,    /* UpArrow on arrow keypad */
	key_prior           = 0xC9,    /* PgUp on arrow keypad */
	key_left            = 0xCB,    /* LeftArrow on arrow keypad */
	key_right           = 0xCD,    /* RightArrow on arrow keypad */
	key_end             = 0xCF,    /* End on arrow keypad */
	key_down            = 0xD0,    /* DownArrow on arrow keypad */
	key_next            = 0xD1,    /* PgDn on arrow keypad */
	key_insert          = 0xD2,    /* Insert on arrow keypad */
	key_delete          = 0xD3,    /* Delete on arrow keypad */
	key_lwin            = 0xDB,    /* Left Windows key */
	key_rwin            = 0xDC,    /* Right Windows key */
	key_apps            = 0xDD,    /* AppMenu key */
	key_power           = 0xDE,    /* System Power */
	key_sleep           = 0xDF,    /* System Sleep */
	key_wake            = 0xE3,    /* System Wake */
	key_websearch       = 0xE5,    /* Web Search */
	key_webfavorites    = 0xE6,    /* Web Favorites */
	key_webrefresh      = 0xE7,    /* Web Refresh */
	key_webstop         = 0xE8,    /* Web Stop */
	key_webforward      = 0xE9,    /* Web Forward */
	key_webback         = 0xEA,    /* Web Back */
	key_mycomputer      = 0xEB,    /* My Computer */
	key_mail            = 0xEC,    /* Mail */
	key_mediaselect     = 0xED,    /* Media Select */

	/*
	 *  Alternate names for keys, to facilitate transition from DOS.
	 */
	key_backspace       = key_back      ,      /* backspace */
	key_numpadstar      = key_multiply  ,      /* * on numeric keypad */
	key_lalt            = key_lmenu     ,      /* left Alt */
	key_capslock        = key_capital   ,      /* CapsLock */
	key_numpadminus     = key_subtract  ,      /* - on numeric keypad */
	key_numpadplus      = key_add       ,      /* + on numeric keypad */
	key_numpadperiod    = key_decimal   ,      /* . on numeric keypad */
	key_numpadslash     = key_divide    ,      /* / on numeric keypad */
	key_ralt            = key_rmenu     ,      /* right Alt */
	key_uparrow         = key_up        ,      /* UpArrow on arrow keypad */
	key_pgup            = key_prior     ,      /* PgUp on arrow keypad */
	key_leftarrow       = key_left      ,      /* LeftArrow on arrow keypad */
	key_rightarrow      = key_right     ,      /* RightArrow on arrow keypad */
	key_downarrow       = key_down      ,      /* DownArrow on arrow keypad */
	key_pgdn            = key_next      ,      /* PgDn on arrow keypad */

	/*
	 *  Alternate names for keys originally not used on US keyboards.
	 */
	key_circumflex      = key_prevtrack ,      /* Japanese keyboard */
}; // enum enum_keyboard

struct XRAY_NOVTABLE keyboard {
public:
	virtual	bool	is_key_down	( enum_keyboard key ) const = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( keyboard )
}; // class handler

} // namespace input
} // namespace xray

#endif // #ifndef XRAY_INPUT_KEYBOARD_H_INCLUDED