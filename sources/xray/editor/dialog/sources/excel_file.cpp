////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "excel_file.h"
#pragma warning(push)
#pragma warning(disable:4564)

using xray::dialog_editor::excel_file;
using namespace Microsoft::Office::Interop::Excel;
using namespace System;

excel_file::excel_file()
{
	m_app = gcnew Microsoft::Office::Interop::Excel::Application();
	m_app->Visible = false;
	m_workbook = nullptr;
	m_worksheet = nullptr;
}

excel_file::~excel_file()
{
	if(m_app!=nullptr)
	{
		//HWND app_hwnd = (HWND)m_app->Hwnd;
		m_app->Quit();
		delete m_app;
		//DWORD pid;
		//GetWindowThreadProcessId(app_hwnd, &pid);
		//HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, TRUE, pid);
		//BOOL result = TerminateProcess(process_handle, 0);
	}
}

void excel_file::create_file(System::String^ fn)
{
	m_file_name = fn;

	System::Globalization::CultureInfo^ old_cult = System::Globalization::CultureInfo::CurrentCulture;
	try
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = System::Globalization::CultureInfo::GetCultureInfo("en-US");
		m_app->SheetsInNewWorkbook = 1;
		m_workbook = m_app->Workbooks->Add(Type::Missing);
		m_worksheet = static_cast<Worksheet^>(m_app->ActiveSheet);
	}
	finally
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = old_cult;
	}
}

void excel_file::load_file(String^ filename)
{
	m_file_name = filename;
	System::Globalization::CultureInfo^ old_cult = System::Globalization::CultureInfo::CurrentCulture;
	try
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = System::Globalization::CultureInfo::GetCultureInfo("en-US");
		m_app->Workbooks->Open(m_file_name, Type::Missing, Type::Missing, Type::Missing, XlPlatform::xlWindows, Type::Missing, Type::Missing, Type::Missing, Type::Missing, Type::Missing, Type::Missing, Type::Missing, Type::Missing, Type::Missing, Type::Missing);
		m_worksheet = static_cast<Worksheet^>(m_app->ActiveSheet);
		R_ASSERT(m_worksheet!=nullptr);
	}
	finally
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = old_cult;
	}
}

void excel_file::save()
{
	if(!m_workbook) 
		return;

	System::Globalization::CultureInfo^ old_cult = System::Globalization::CultureInfo::CurrentCulture;
	try
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = System::Globalization::CultureInfo::GetCultureInfo("en-US");
		m_workbook->SaveAs(m_file_name, XlFileFormat::xlWorkbookDefault, nullptr, nullptr, nullptr, nullptr, XlSaveAsAccessMode::xlExclusive, nullptr, nullptr, nullptr, nullptr, true);
	}
	finally
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = old_cult;
	}

	return;
}

void excel_file::write_value(String^ item, String^ value, bool bold, bool italic)
{
	write(item, value, bold, italic);
}

void excel_file::write_value(String^ item, String^ value)
{
	write_value(item, value, false, false);
}

void excel_file::write_value(String^ item, s32 value)
{
	Int32 val = value;
	write_value(item, val.ToString(), false, false);
}

void excel_file::write(String^ item, String^ value, bool bold, bool italic)
{
	System::Globalization::CultureInfo^ old_cult = System::Globalization::CultureInfo::CurrentCulture;
	try
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = System::Globalization::CultureInfo::GetCultureInfo("en-US");
		Range^ range = m_worksheet->Range[item, item];
		if(range==nullptr) 
			return;

		range->Font->Bold = bold;
		range->Font->Italic = italic;
		range->Value[XlRangeValueDataType::xlRangeValueDefault] = value;
	}
	finally
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = old_cult;
	}
}

String^ excel_file::read_string(String^ item)
{
	return read(item);
}

s32 excel_file::read_s32(String^ item)
{
	System::String^ val = read(item);
	return Convert::ToInt32(val);
}

String^ excel_file::read(String^ item)
{
	System::String^ str;
	System::Globalization::CultureInfo^ old_cult = System::Globalization::CultureInfo::CurrentCulture;
	try
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = System::Globalization::CultureInfo::GetCultureInfo("en-US");
		str = dynamic_cast<System::String^>(m_worksheet->Range[item, item]->Value[XlRangeValueDataType::xlRangeValueDefault]);
	}
	finally
	{
		System::Threading::Thread::CurrentThread->CurrentCulture = old_cult;
	}
	if(str==nullptr) 
		return "";

	return str;
}
#pragma warning(pop)