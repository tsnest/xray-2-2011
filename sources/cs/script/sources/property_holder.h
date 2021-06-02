////////////////////////////////////////////////////////////////////////////
//	Module 		: property_holder.h
//	Created 	: 12.11.2005
//  Modified 	: 12.11.2005
//	Author		: Dmitriy Iassenev
//	Description : property holder class
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_H_INCLUDEDOLDER_H_INCLUDED
#define PROPERTY_H_INCLUDEDOLDER_H_INCLUDED

class string_stream;

namespace cs {
namespace script {
namespace detail {

class world;

#pragma warning(push)
#pragma warning(disable:4275)
class property_holder :
	public cs::script::property_holder,
	private boost::noncopyable
{
#pragma warning(pop)
private:
	friend struct reference_section_predicate;

private:
	typedef cs::script::property_holder			inherited;

public:
	typedef inherited::object_type				object_type;
	typedef luabind::iterator					iterator;

private:
	world&						m_world;
	object_type					m_object;											// former "DATA"
#pragma warning(push)
#pragma warning(disable:4251)
	str_shared					m_id;												// former "fName"
#pragma warning(pop)
	u8							m_flags;

private:
	template <typename T, typename P>
			void				w						(pcstr S, pcstr L, T V, pcstr comment, P const& predicate);

#if 0
	template <typename type>
			void				w_vector2				(pcstr S, pcstr L, type const& V, pcstr comment);

	template <typename type>
			void				w_vector3				(pcstr S, pcstr L, type const& V, pcstr comment);

	template <typename type>
			void				w_vector4				(pcstr S, pcstr L, type const& V, pcstr comment);
#endif // #if 0

			void				save					(lua_State* state, string_stream& stream, pstr indent, u32 indent_size, pstr previous_indent, u32 level);
			object_type			create_section_object	(pcstr section) const;

public:
			bool				table				(object_type const& section) const;
			object_type			section_object		(pcstr section) const;

private:
	template <typename _result_type>
	inline	_result_type		value				(pcstr section, pcstr line) const;
	inline	object_type			value				(pcstr section, pcstr line) const;
			void				construct			(pcstr file_name, u8 const flags);
			bool				is_section			(object_type object) const;
			void				code_table			(pcstr body);

public:
								property_holder		(world* world, pcstr file_name, u8 const flags = (inherited::read_only | inherited::load_on_construct | inherited::save_on_destruct));	// former "CInifile"
	inline	pcstr				id					() const;
	inline	object_type const&	object				() const;

//	obsolete functions
public:
	static	inline	bool		bool_value			(pcstr string);
	inline	object_type const&	sections			() const;

	virtual bool				section_exist		(pcstr section) const;
	virtual bool				line_exist			(pcstr section, pcstr line) const;
	virtual u32					line_count			(pcstr section) const;
	virtual object_type			r_section			(pcstr section) const;
	virtual bool				r_line				(pcstr section, int line_id, pcstr& id, pcstr& value) const;
	virtual	object_type			r_object			(pcstr section, pcstr line) const;

	virtual pcstr				r_code				(pcstr section, pcstr line) const;
	virtual pcstr				r_string			(pcstr section, pcstr line) const;
	virtual bool				r_bool				(pcstr section, pcstr line) const;
	virtual u8					r_u8				(pcstr section, pcstr line) const;
	virtual u16					r_u16				(pcstr section, pcstr line) const;
	virtual u32					r_u32				(pcstr section, pcstr line) const;
	virtual s8					r_s8				(pcstr section, pcstr line) const;
	virtual s16					r_s16				(pcstr section, pcstr line) const;
	virtual s32					r_s32				(pcstr section, pcstr line) const;
	virtual float				r_float				(pcstr section, pcstr line) const;

	virtual void				w_code				(pcstr S, pcstr L, pcstr V, pcstr comment=0);
	virtual void				w_string			(pcstr S, pcstr L, pcstr V, pcstr comment=0);
	virtual void				w_bool				(pcstr S, pcstr L, bool V, pcstr comment=0);
	virtual	void				w_u8				(pcstr S, pcstr L, u8 V, pcstr comment=0);
	virtual void				w_u16				(pcstr S, pcstr L, u16 V, pcstr comment=0);
	virtual void				w_u32				(pcstr S, pcstr L, u32 V, pcstr comment=0);
	virtual void				w_s8				(pcstr S, pcstr L, s8 V, pcstr comment=0);
	virtual void				w_s16				(pcstr S, pcstr L, s16 V, pcstr comment=0);
	virtual void				w_s32				(pcstr S, pcstr L, s32 V, pcstr comment=0);
	virtual void				w_float				(pcstr S, pcstr L, float V, pcstr comment=0);

	virtual void				remove_line			(pcstr S, pcstr L);
	virtual void				remove_section		(pcstr S);
	virtual void				rename_section		(pcstr old_section_id, pcstr new_section_id);
	virtual void				reference_section	(pcstr old_section_id, pcstr new_section_id);
	virtual void				setup_bases			(pcstr S, pcstr L);
	virtual void				save_as				(pcstr file_name);
};

} // namespace detail
} // namespace script
} // namespace cs

#include "property_holder_inline.h"

#endif // #ifndef PROPERTY_H_INCLUDEDOLDER_H_INCLUDED