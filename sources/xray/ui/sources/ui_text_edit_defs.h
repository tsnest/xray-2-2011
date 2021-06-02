////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef UI_TEXT_EDIT_DEFS_H_INCLUDED
#define UI_TEXT_EDIT_DEFS_H_INCLUDED

namespace xray {
namespace ui {

enum enum_cursor_moving{
	cr_left,
	cr_right,
	cr_begin,
	cr_end
};

enum enum_shift_state
{
	ks_Shift,
	ks_Ctrl,
	ks_Alt,
	ks_CtrlShift,
};

struct shift_state
{
	union data_storage
	{
		struct {
		u8	ctrl	:2;
		u8	alt		:2;
		u8	shift	:2;
		u8	force	:2;
		}d;
		u8	dummy;
	};
	data_storage	m_data;
	shift_state&	set_any	();
	shift_state&	set_none(){m_data.dummy=0x0; return *this;}
};

enum enum_text_edit_mode
{
	te_standart = 0,
	te_number_only,
	te_read_only,
	te_file_name_mode, // not "/\\:*?\"<>|^()[]%" 
};

} // namespace xray
} // namespace ui

#endif //UI_TEXT_EDIT_DEFS_H_INCLUDED