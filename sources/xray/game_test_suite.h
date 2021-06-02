////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_GAME_TEST_SUITE_H_INCLUDED
#define XRAY_GAME_TEST_SUITE_H_INCLUDED

#include <xray/testing.h>
#include <xray/linkage_helper.h>



namespace xray {

namespace animation
{
	struct world;
} // namespace animation

namespace rtp
{
	struct world;
}

class XRAY_CORE_API game_test_suite : public testing::suite_base<game_test_suite>
{

public:
	game_test_suite(): m_rtp_world( 0 ) {}

	void			set_rtp_world	( rtp::world * rtp_world )	{  m_rtp_world = rtp_world; }

	
	rtp::world *	rtp_world		( )const					{ return m_rtp_world; }
	//rtp::world *	rtp_world		( )							{ return m_rtp_world; }

public:
	DEFINE_SUITE_HELPERS

private:
	rtp::world *m_rtp_world;

}; // class game_test_suite

} // namespace xray

#endif // #ifndef XRAY_GAME_TEST_SUITE_H_INCLUDED