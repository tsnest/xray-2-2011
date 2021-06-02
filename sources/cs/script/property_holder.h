////////////////////////////////////////////////////////////////////////////
//	Module 		: property_holder.h
//	Created 	: 16.11.2005
//  Modified 	: 30.03.2009
//	Author		: Dmitriy Iassenev
//	Description : property holder interface
////////////////////////////////////////////////////////////////////////////

#ifndef CS_SCRIPT_PROPERTY_H_INCLUDEDOLDER_H_INCLUDED
#define CS_SCRIPT_PROPERTY_H_INCLUDEDOLDER_H_INCLUDED

#include <cs/script/interfaces.h>

namespace cs {
namespace script {

struct DECLSPEC_NOVTABLE property_holder {
public:
	typedef luabind::object		object_type;

public:
	enum {
		read_only				= u8(1 << 0),
		load_on_construct		= u8(1 << 1),
		save_on_destruct		= u8(1 << 2),
		do_not_convert			= u8(1 << 3),
	};

public:
	virtual	object_type const&	sections			() const = 0;

	// pcstr version
	virtual	bool				section_exist		(pcstr section) const = 0;
	virtual	bool				line_exist			(pcstr section, pcstr line) const = 0;
	virtual	u32					line_count			(pcstr section) const = 0;
	virtual	object_type			r_section			(pcstr section) const = 0;
	virtual	bool				r_line				(pcstr section, int line_id, pcstr& id, pcstr& value) const = 0;
	virtual	object_type			r_object			(pcstr section, pcstr line) const = 0;

	virtual pcstr				r_code				(pcstr section, pcstr line) const = 0;
	virtual	pcstr				r_string			(pcstr section, pcstr line) const = 0;
	virtual	bool				r_bool				(pcstr section, pcstr line) const = 0;
	virtual	u8					r_u8				(pcstr section, pcstr line) const = 0;
	virtual	u16					r_u16				(pcstr section, pcstr line) const = 0;
	virtual	u32					r_u32				(pcstr section, pcstr line) const = 0;
	virtual	s8					r_s8				(pcstr section, pcstr line) const = 0;
	virtual	s16					r_s16				(pcstr section, pcstr line) const = 0;
	virtual	s32					r_s32				(pcstr section, pcstr line) const = 0;
	virtual	float				r_float				(pcstr section, pcstr line) const = 0;
	// write functions
	virtual void				w_code				(pcstr S, pcstr L, pcstr V, pcstr comment=0) = 0;
	virtual void				w_string			(pcstr S, pcstr L, pcstr V, pcstr comment=0) = 0;
	virtual void				w_bool				(pcstr S, pcstr L, bool V, pcstr comment=0) = 0;
	virtual	void				w_u8				(pcstr S, pcstr L, u8 V, pcstr comment=0) = 0;
	virtual void				w_u16				(pcstr S, pcstr L, u16 V, pcstr comment=0) = 0;
	virtual void				w_u32				(pcstr S, pcstr L, u32 V, pcstr comment=0) = 0;
	virtual void				w_s8				(pcstr S, pcstr L, s8 V, pcstr comment=0) = 0;
	virtual void				w_s16				(pcstr S, pcstr L, s16 V, pcstr comment=0) = 0;
	virtual void				w_s32				(pcstr S, pcstr L, s32 V, pcstr comment=0) = 0;
	virtual void				w_float				(pcstr S, pcstr L, float V, pcstr comment=0) = 0;

	virtual void				remove_line			(pcstr S, pcstr L) = 0;
	virtual void				remove_section		(pcstr S) = 0;
	virtual void				rename_section		(pcstr old_section_id, pcstr new_section_id) = 0;
	virtual void				reference_section	(pcstr old_section_id, pcstr new_section_id) = 0;
	virtual void				setup_bases			(pcstr S, pcstr L) = 0;
	virtual void				save_as         	(pcstr file_name) = 0;

	CS_DECLARE_PURE_VIRTUAL_DESTRUCTOR( property_holder )

public:
	template <typename _result_type>
	static inline _result_type	cast				(object_type object)
	{
		return					luabind::object_cast<_result_type>(object);
	}
}; // struct property_holder

} // namespace script
} // namespace cs

#endif // #ifndef CS_SCRIPT_PROPERTY_H_INCLUDEDOLDER_H_INCLUDED