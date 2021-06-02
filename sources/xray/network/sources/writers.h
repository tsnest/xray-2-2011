////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef WRITERS_H_INCLUDED
#define WRITERS_H_INCLUDED

#include "writer.h"
#include "buffer_writer_device.h"

namespace xray
{

typedef writer<buffer_writer_device>		buffer_writer;

} // namespace xray

#endif // #ifndef WRITERS_H_INCLUDED