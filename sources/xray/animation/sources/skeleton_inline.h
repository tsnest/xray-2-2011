////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace animation {

inline bone	const& 	skeleton::get_bone( bone_index_type idx ) const
{
	ASSERT( idx < m_bone_count );
	return m_bones[idx];
}

} // namespace animation
} // namespace xray