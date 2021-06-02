////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OPERATORS_H_INCLUDED
#define OPERATORS_H_INCLUDED

namespace xray
{
	public ref class dot_net
	{
	public:
		generic<class TType, class TObj>
		static bool	is	( TObj obj )
		{
			return xray::editor::wpf_controls::dot_net_helpers::is_type<TType>( obj );
		}

		generic<class TType, class TObj> where TType: ref class
		static TType	as	( TObj obj )
		{
			return xray::editor::wpf_controls::dot_net_helpers::as_type<TType>( obj );
		}
	}; //ref class dot_net

} // namespace xray

#endif // #ifndef OPERATORS_H_INCLUDED