////////////////////////////////////////////////////////////////////////////
//	Module 		: fs_readers_base.h
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system reader base class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_FS_READERS_BASE_H_INCLUDED
#define CS_CORE_FS_READERS_BASE_H_INCLUDED

#include <cs/gtl/intrusive_ptr.h>
#include <boost/noncopyable.hpp>

namespace fs { namespace readers {

#pragma warning(push)
#pragma warning(disable:4275)
class CS_CORE_API	base : 
	public gtl::intrusive_base,
	public boost::noncopyable
{
#pragma warning(pop)
private:
	pcvoid				m_buffer;
	u32					m_buffer_size;
	u32					m_position;

public:
	inline				base				(pcvoid buffer = 0, u32 const buffer_size = 0, u32 const position = 0);
	inline	virtual		~base				();
	inline	void		setup				(pcvoid buffer, u32 const buffer_size, u32 const position);
	inline	bool		terminal_character	(char const& character) const;
			u32			advance_string		();
	inline	pcstr		r_string			(pstr destination, u32 const max_length);
	template <int size>
	inline	pcstr		r_string			(char (&destination)[size]);
	inline	bool		eof					() const;
	inline	u32 	length				() const;
	inline	u32 	tell				() const;
	inline	pcvoid		pointer				() const;
	inline	void		advance				(u32 const size);
	inline	void		seek				(u32 const size);
	inline	void		r					(pvoid buffer, u32 const buffer_size);
};

}

typedef gtl::intrusive_ptr<fs::readers::base>	reader;

}

#include "fs_readers_base_inline.h"

#endif // #ifndef CS_CORE_FS_READERS_BASE_H_INCLUDED