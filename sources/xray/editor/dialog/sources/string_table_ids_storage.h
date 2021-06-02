////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef STRING_TABLE_IDS_STORAGE_H_INCLUDED
#define STRING_TABLE_IDS_STORAGE_H_INCLUDED

#pragma managed( push, off )

namespace xray {
namespace dialog_editor {
namespace string_tables {
	class string_table_id;
	class string_table_ids_storage;
	struct strings_compare_predicate
	{
		inline bool	operator()			(pcstr s1, pcstr s2) const
		{
			return		( strings::compare(s1, s2) < 0 );
		}
	}; // struct strings_compare_predicate

	class string_table_ids_storage : private boost::noncopyable
	{
	public:
		typedef map<pstr, string_table_id*, strings_compare_predicate> string_table_ids_type;
		typedef set<pstr, strings_compare_predicate> localization_languages_type;
		
	public:
								string_table_ids_storage	();
								~string_table_ids_storage	();
				void			add_new_id					(pcstr new_id, pcstr new_text);
				void			change_ids_text				(pcstr id, pcstr new_text);
				pcstr			get_text_by_id				(pcstr str_id);
				void			set_cur_language_name		(pcstr new_lang);
				pcstr			cur_language_name			();
const localization_languages_type*	languages_list			() const		{return m_languages;};
const	string_table_ids_type*	string_table_ids			() const		{return m_string_table_ids;};
				bool			check_id_on_empty_text		(pcstr id);
				void			new_language				(pcstr lang_name);
				void			remove_language				(pcstr lang_name);

	private:
				void			on_loc_table_loaded			(xray::resources::queries_result& data);
				void			on_string_table_ids_loaded	(xray::resources::queries_result& data);

	private:
		pstr							m_cur_language;
		string_table_ids_type*			m_string_table_ids;
		localization_languages_type*	m_languages;
	}; // class string_table_ids_storage


	string_table_ids_storage*	create_string_tables	();
	string_table_ids_storage*	get_string_tables		();
	void						destroy_string_tables	();

} // namespace string_tables 
} // namespace dialog_editor 
} // namespace xray 
#pragma managed( pop )

#endif // #ifndef STRING_TABLE_IDS_STORAGE_H_INCLUDED
