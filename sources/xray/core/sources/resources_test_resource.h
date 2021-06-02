////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_TEST_RESOURCE_H_INCLUDED
#define RESOURCES_TEST_RESOURCE_H_INCLUDED

#include <xray/fs/path_string_utils.h>

namespace xray {
namespace resources {

enum { generate_file_data_size	=	128, creation_data_size	=	250	 };
enum creation_enum { creation_from_file, creation_from_memory, creation_inplace_from_file, creation_inplace_from_memory, creation_translate_query };

void   fill_with_generated_file_data (mutable_buffer generated_file_data)
{
	TEST_CURE_ASSERT_CMP			(generated_file_data.size(), ==, (u32)generate_file_data_size, return);
	for ( u32 i=0; i<generated_file_data.size(); ++i )
		generated_file_data[i]	=	(char)(i + 1);
}

void   test_generated_file_data (mutable_buffer generated_file_data)
{
	TEST_CURE_ASSERT_CMP					(generated_file_data.size(), ==, (u32)generate_file_data_size, return);
	mutable_buffer							etalon(ALLOCA(generate_file_data_size), generate_file_data_size);
	fill_with_generated_file_data			(etalon);
	TEST_CURE_ASSERT						(memory::equal(etalon, generated_file_data), return);
}

class test_managed_resource 
{
public:
	test_managed_resource (pcstr path, mutable_buffer file_data, creation_enum creation) : m_path(path), m_file_data(file_data), m_creation(creation)
	{
		for ( u32 i=0; i<array_size(m_class_data); ++i )
			m_class_data[i]	=	(u8)i;
	}

	void		test_consistency () const
	{
		test_managed_resource	test		("", mutable_buffer(0, 0), creation_from_file);
		TEST_ASSERT							(memory::equal(m_class_data, test.m_class_data));

		if ( m_creation == creation_translate_query )
			return;

		if ( m_creation == creation_from_memory || m_creation == creation_inplace_from_memory )
		{
			TEST_CURE_ASSERT_CMP			(m_file_data.size(), ==, (u32)creation_data_size, return);
			for ( u32 i=0; i<creation_data_size; ++i )
				TEST_CURE_ASSERT_CMP		(m_file_data[i], ==, (i % 2) ? '0' : '1', return);
			return;
		}

		if ( m_creation == creation_inplace_from_file )
		{
			for ( u32 i=0; i<m_file_data.size(); ++i )
				TEST_CURE_ASSERT_CMP		(m_file_data[i], ==, 'i', return);
			return;
		}

		fs_new::virtual_path_string			file_name;
		fs_new::file_name_with_no_extension_from_path	(& file_name, m_path.c_str());

		bool const generated_if_no_file	=	file_name.find("this_file_dont_exist") != u32(-1);

		if ( generated_if_no_file )
		{
			test_generated_file_data		(m_file_data);
			return;
		}

		u32 const separator_pos			=	(u32)file_name.find('-');
		if ( separator_pos == u32(-1) )
			return;

		fixed_string512						data_string, count_string;
		file_name.substr					(0, separator_pos, & data_string);
		file_name.substr					(separator_pos + 1, u32(-1), & count_string);
		u32 count;
		strings::convert_string_to_number	(count_string.c_str(), & count);
				
		TEST_CURE_ASSERT_CMP				(count, ==, m_file_data.size(), return);

		for ( u32 i=0; i<count; ++i )
			TEST_CURE_ASSERT_CMP			(m_file_data[i], ==, data_string[0], break);
	}

	mutable_buffer		get_file_data ()	{ return m_file_data; }

private:
	fs_new::virtual_path_string			m_path;
	char								m_class_data[128];
	mutable_buffer						m_file_data;
	creation_enum						m_creation;
};

class test_unmanaged_resource : public unmanaged_resource, 
								public test_managed_resource
{
public:
	test_unmanaged_resource	(pcstr path, mutable_buffer file_data, creation_enum creation)
		: test_managed_resource (path, file_data, creation) {}
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_TEST_RESOURCE_H_INCLUDED