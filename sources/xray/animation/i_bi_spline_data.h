////////////////////////////////////////////////////////////////////////////
//	Created		: 16.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_BI_SPLINE_DATA_H_INCLUDED
#define I_BI_SPLINE_DATA_H_INCLUDED

namespace xray {
namespace animation {

template<typename Point_type>
struct i_bi_spline_data
{
	virtual void	add_point	(  const std::pair<float, Point_type> &p ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( i_bi_spline_data )
};

} // namespace  animation
} // namespace  xray


#endif // #ifndef BI_SPLINE_DATA_H_INCLUDED