////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef STRING_TABLE_ID_H_INCLUDED
#define STRING_TABLE_ID_H_INCLUDED

#pragma managed( push, off )

namespace xray {
namespace dialog_editor {
namespace string_tables {
	struct strings_compare_predicate;

	class string_table_id
	{
	public:
		typedef map<pcstr, pstr, strings_compare_predicate> localizations_type;

	public:
				string_table_id		();
				~string_table_id	();
		void	new_text			(pcstr loc_name, pcstr new_txt);
		void	change_text			(pcstr new_txt);
		void	remove_text			(pcstr loc_name);
		pcstr	text				();
		pcstr	text_by_loc_name	(pcstr loc_name);
		bool	check_on_empty_text	();

	private:
		localizations_type*	m_localizations;
	}; // class string_table_id
} // namespace string_tables 
} // namespace dialog_editor 
} // namespace xray 
#pragma managed( pop )

#endif // #ifndef STRING_TABLE_ID_H_INCLUDED
