////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FLOAT3_SIMILAR_H_INCLUDED
#define FLOAT3_SIMILAR_H_INCLUDED

namespace xray {
namespace model_editor {

struct float3_similar: public std::binary_function<math::float3 const&, math::float3 const&, bool>
{
	float3_similar( float precision = math::epsilon_5 ):
		m_precision(precision){}
	bool operator() ( float3 const& first, float3 const& second ) const
	{
		return math::is_similar( first, second, m_precision );
	}
private:
	float m_precision;
};

} // namespace model_editor
} // namespace xray

#endif // #ifndef FLOAT3_SIMILAR_H_INCLUDED