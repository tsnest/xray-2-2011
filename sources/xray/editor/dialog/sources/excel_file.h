////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef EXCEL_FILE_H_INCLUDED
#define EXCEL_FILE_H_INCLUDED

using namespace Microsoft::Office::Interop::Excel;
using namespace System;

namespace xray {
namespace dialog_editor {
	public ref class excel_file
	{
	public:
				excel_file	();
				~excel_file	();

		void	create_file	(System::String^ fn);
		void	load_file	(String^ filename);
		void	save		();
		
		void	write_value	(String^ item, String^ value, bool bold, bool italic);
		void	write_value	(String^ item, String^ value);
		void	write_value	(String^ item, s32 value);

		String^ read_string	(String^ item);
		s32		read_s32	(String^ item);

	private:
		void	write		(String^ item, String^ value, bool bold, bool italic);
		String^ read		(String^ item);

	private:
		Microsoft::Office::Interop::Excel::Application^	m_app;
		Microsoft::Office::Interop::Excel::Workbook^	m_workbook;
		Microsoft::Office::Interop::Excel::Worksheet^	m_worksheet;
		Microsoft::Office::Interop::Excel::Font^		m_write_font;
		Microsoft::Office::Interop::Excel::Font^		m_default_font;
		System::String^									m_file_name;
	}; // ref class excel_file
} // namespace dialog_editor
} // namespace xray
#endif // #ifndef EXCEL_FILE_H_INCLUDED