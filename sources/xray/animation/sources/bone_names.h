////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BONE_NAMES_H_INCLUDED
#define BONE_NAMES_H_INCLUDED

#include <xray/shared_string.h>
#include <xray/animation/i_bone_names.h>

namespace xray {

namespace configs{
	class lua_config_value;
} // namespace configs

namespace animation {

class skeleton;

class bone_names:
	public i_bone_names
{
private:
	typedef fixed_string< 64 >	string_type;

public:
	virtual	void			set_bones_number( bone_index_type size );
	virtual void			set_name		( bone_index_type bone_index, pcstr name );

public:	
	virtual void			read			( xray::configs::lua_config_value const &cfg );
	virtual void			write			( xray::configs::lua_config_value		&cfg )const;

public:
			void			create_index	( const skeleton &skel, vector< bone_index_type > &index )const;

private:
			bone_index_type	bone_index		( const string_type &name )const;

private:
	vector< string_type >	m_bone_names;

}; // class bone_names

} // namespace animation
} // namespace xray

#endif // #ifndef BONE_NAMES_H_INCLUDED