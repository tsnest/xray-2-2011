////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////


#ifndef TEST_APPLICATION_H_INCLUDED
#define TEST_APPLICATION_H_INCLUDED

class application 
{
public:
			void	initialize		( u32 argc, pstr const * argv );
			void	execute			( );
			void	finalize		( );
			void    test_compression( pvoid );
			void	test_allocator	( );
	inline	u32		get_exit_code	( ) const { return m_exit_code; }
			void	compression_unit( xray::memory::base_allocator* allocator );

private:
	HWND			m_splash_screen;
	pcstr const*	m_argv;
	bool			m_helper_thread_must_exit;
	u32				m_argc;

private:
	u32				m_exit_code;
}; // class application

#endif // #ifndef TEST_APPLICATION_H_INCLUDED