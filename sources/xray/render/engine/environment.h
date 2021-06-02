////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ENVIRONMENT_H_INCLUDED
#define ENVIRONMENT_H_INCLUDED

namespace xray {
namespace render_dx10 {

class environment: public quasi_singleton<environment>
{
public:
	float4 get_amb_color() {return float4(0.08f, 0.08f, 0.08f, 0);}
	float4 get_env_color() {return float4(0.46055603f, 0.38911888f, 0.35198119f, 1);}
	float4 get_fog_color() {return float4(0.26033184f, 0.27797237f, 0.38404618f, 0);}

private:

}; // class environment

} // namespace render
} // namespace xray


#endif // #ifndef ENVIRONMENT_H_INCLUDED