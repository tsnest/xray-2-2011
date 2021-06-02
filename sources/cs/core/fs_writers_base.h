////////////////////////////////////////////////////////////////////////////
//	Module 		: fs_writers_base.h
//	Created 	: 02.09.2006
//  Modified 	: 02.09.2006
//	Author		: Dmitriy Iassenev
//	Description : file system writer base class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_FS_WRITERS_BASE_H_INCLUDED
#define CS_CORE_FS_WRITERS_BASE_H_INCLUDED

#include <cs/gtl/intrusive_ptr.h>
#include <boost/noncopyable.hpp>

namespace fs { namespace writers {

#pragma warning(push)
#pragma warning(disable:4275)
class CS_CORE_API	base : 
	public ::gtl::intrusive_base,
	public ::boost::noncopyable
{
#pragma warning(pop)
public:
	virtual	void	w		(pcvoid buffer, u32 const buffer_size) = 0;
	virtual			~base	() {}
};

}

typedef gtl::intrusive_ptr<fs::writers::base>	writer;

}

#endif // #ifndef CS_CORE_FS_WRITERS_BASE_H_INCLUDED