////////////////////////////////////////////////////////////////////////////
//	Created		: 22.06.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CORE_TEST_SUITE_H_INCLUDED
#define XRAY_CORE_TEST_SUITE_H_INCLUDED

#include <xray/testing.h>
#include <xray/linkage_helper.h>
#include <xray/fs/native_path_string.h>

XRAY_INCLUDE_TO_LINKAGE(resources_test);
XRAY_INCLUDE_TO_LINKAGE(compressor_test);
XRAY_INCLUDE_TO_LINKAGE(threading_test);
XRAY_INCLUDE_TO_LINKAGE(math_tests);
XRAY_INCLUDE_TO_LINKAGE(game_resources_manager_test);
XRAY_INCLUDE_TO_LINKAGE(resources_fs_task_erase);

namespace xray {

class XRAY_CORE_API core_test_suite : public testing::suite_base<core_test_suite>
{
public:
	DEFINE_SUITE_HELPERS

public:
	void	set_resources_path	(pcstr resources_path)	{ m_resources_path.assign_with_conversion(resources_path); }
	pcstr	get_resources_path	() const				{ return m_resources_path.c_str(); }

private:
	fs_new::native_path_string		m_resources_path;
}; // class core_test_suite

} // namespace xray

#endif // #ifndef XRAY_CORE_TEST_SUITE_H_INCLUDED