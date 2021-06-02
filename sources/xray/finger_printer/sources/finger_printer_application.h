////////////////////////////////////////////////////////////////////////////
//	Created 	: 26.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

class application 
{
public:
			void	initialize		();
			void	execute			();
			void	finalize		();
			void    test_compression (pvoid);
			void	test_allocator	();
	inline	u32		return_code		() const { return m_return_code; }
			void	compression_unit	(xray::memory::base_allocator*	allocator);
			void    do_finger_printing  (xray::fixed_string512 const &	file_name, 
										 xray::fixed_string512 const &	finger_print_message, 
										 pstr 							mark1, 
										 pstr 							mark2, 
										 u32							build_id, 
										 pstr							file_data, 
										 u32							file_size);

private:
	HWND								m_splash_screen;
	pcstr const*						m_argv;
	bool								m_helper_thread_must_exit;
	u32									m_argc;

private:
	u32		m_return_code;
}; // class application


#endif // #ifndef APPLICATION_H_INCLUDED