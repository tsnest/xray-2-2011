////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FS_PACK_APPLICATION_H_INCLUDED
#define FS_PACK_APPLICATION_H_INCLUDED

namespace fs_pack {

class application 
{
public:
			void	initialize		( u32 argc, pstr const * argv );
			void	execute			( );
			void	finalize		( );
	inline	u32		get_exit_code	( ) const { return m_exit_code; }

private:
	HWND			m_splash_screen;
	pcstr const*	m_argv;
	u32				m_argc;

private:
	u32				m_exit_code;
}; // class application


} // namespace fs_pack

#endif // #ifndef FS_PACK_APPLICATION_H_INCLUDED