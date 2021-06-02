////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ENGINE_TEST_SUITE_H_INCLUDED
#define XRAY_ENGINE_TEST_SUITE_H_INCLUDED

#include <xray/testing.h>
#include <xray/linkage_helper.h>
#include <xray/engine/api.h>

namespace xray {

class XRAY_CORE_API engine_test_suite : public testing::suite_base<engine_test_suite>
{
public:
	DEFINE_SUITE_HELPERS

}; // class engine_test_suite

} // namespace xray

#endif // #ifndef XRAY_ENGINE_TEST_SUITE_H_INCLUDED