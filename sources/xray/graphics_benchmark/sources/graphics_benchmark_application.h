////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPHICS_BENCHMARK_APPLICATION_H_INCLUDED
#define GRAPHICS_BENCHMARK_APPLICATION_H_INCLUDED

namespace xray {
namespace graphics_benchmark {

class application {
public:
			void	initialize		( );
			void	execute			( );
			void	finalize		( );
	inline	u32		get_exit_code	( ) const	{ return m_exit_code; }

private:
	u32		m_exit_code;
}; // class application

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef GRAPHICS_BENCHMARK_APPLICATION_H_INCLUDED