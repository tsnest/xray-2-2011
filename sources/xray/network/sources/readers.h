////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef READERS_H_INCLUDED
#define READERS_H_INCLUDED

#include "reader.h"
#include "buffer_reader_device.h"

namespace xray
{

typedef reader<buffer_reader_device>	buffer_reader;

} // namespace xray

#endif // #ifndef READERS_H_INCLUDED