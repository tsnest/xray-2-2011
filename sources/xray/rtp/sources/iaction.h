////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef IACTION_H_INCLUDED
#define IACTION_H_INCLUDED

template< class space_params >
class iaction
{
public:
	virtual				~iaction			( ) { }
	virtual		void	render				( const space_params& a,  xray::render::debug::renderer& renderer ) const	=0;
};


#endif // #ifndef IACTION_H_INCLUDED