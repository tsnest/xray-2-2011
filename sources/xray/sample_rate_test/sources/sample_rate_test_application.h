////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SAMPLE_RATE_TEST_APPLICATION_H_INCLUDED
#define SAMPLE_RATE_TEST_APPLICATION_H_INCLUDED


namespace xray {
namespace sample_rate_test {

class application {
public:
			void	initialize		( );
			void	execute			( );
			void	finalize		( );
	inline	u32		get_exit_code	( ) const	{ return m_exit_code; }

private:
	u32		m_exit_code;
}; // class application

} // namespace sample_rate_test
} // namespace xray

#endif // #ifndef SAMPLE_RATE_TEST_APPLICATION_H_INCLUDED