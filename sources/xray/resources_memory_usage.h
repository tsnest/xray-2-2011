////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_MEMORY_USAGE_H_INCLUDED
#define XRAY_RESOURCES_MEMORY_USAGE_H_INCLUDED

namespace xray {
namespace resources {

class memory_type;

class XRAY_CORE_API memory_usage_type
{
public:
	memory_usage_type	() : type(0), size(0) { ; }
	memory_usage_type	(memory_type const * type, u32 size) : type(type), size(size) { ; }
	memory_usage_type	(memory_type const & type, u32 size) : type(& type), size(size) { ; }

	fixed_string512		log_string			() const;
	bool				same_type_with		(memory_usage_type const & other) const { return type == other.type; }

public:
	memory_type const *	type;
	u32					size;
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_MEMORY_USAGE_H_INCLUDED