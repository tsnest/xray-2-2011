////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export.h
//	Created 	: 03.07.2005
//  Modified 	: 10.04.2006
//	Author		: Dmitriy Iassenev
//	Description : script export functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_SCRIPT_EXPORT_H_INCLUDED
#define CS_GTL_SCRIPT_EXPORT_H_INCLUDED

#ifndef CS_SCRIPT_EXPORT_HEADER
	STATIC_CHECK	(false,Please_define_SCRIPT_EXPORT_H_INCLUDEDEADER_macros_with_the_header_which_holds_all_the_includes_for_script_export)
#endif // #ifndef CS_SCRIPT_EXPORT_HEADER

#ifndef CS_ON_BEFORE_SCRIPT_EXPORT_CALL
#	define CS_ON_BEFORE_SCRIPT_EXPORT_CALL(a)
#endif // #ifndef CS_ON_BEFORE_SCRIPT_EXPORT_CALL

#ifndef CS_ON_AFTER_SCRIPT_EXPORT_CALL
#	define CS_ON_AFTER_SCRIPT_EXPORT_CALL(a)
#endif // #ifndef CS_ON_AFTER_SCRIPT_EXPORT_CALL

#define TL_FAST_COMPILATION

//#ifdef STATIC_CHECK
//#	undef STATIC_CHECK
//#endif // #ifdef STATIC_CHECK
//#include <loki/typelist.h>
#define ASSERT_TYPELIST(...)

#define CS_SCRIPT_EXPORTING
#	include <cs/gtl/script_export_macroses.h>
#	include CS_SCRIPT_EXPORT_HEADER
#undef  CS_SCRIPT_EXPORTING

namespace cs { namespace CS_SCRIPT_USER {

namespace gtl { namespace detail {

template <typename type_list>
struct script_exporter
{
	ASSERT_TYPELIST(type_list);

	inline static void process	(lua_State* state)
	{
		script_exporter<typename type_list::Tail>::process	(state);

		CS_ON_BEFORE_SCRIPT_EXPORT_CALL	(typeid(type_list::Head).name());
		type_list::Head::script_export	(state);
		CS_ON_AFTER_SCRIPT_EXPORT_CALL	(typeid(type_list::Head).name());
	}
};

template <>
struct script_exporter<Loki::NullType>
{
	static void process	(lua_State* state)
	{
		CS_UNREFERENCED_PARAMETER	( state );
	}
};

} // namespace detail

inline void script_export		(lua_State* state)
{
	detail::script_exporter<CS_SCRIPT_EXPORT_TYPE_LIST>::process(state);
}

} // namespace gtl
} // namespace CS_SCRIPT_USER
} // namespace cs

#undef CS_SCRIPT_USER
#undef CS_SCRIPT_EXPORT_HEADER
#undef TL_FAST_COMPILATION
#undef CS_ON_BEFORE_SCRIPT_EXPORT_CALL
#undef CS_ON_AFTER_SCRIPT_EXPORT_CALL

#endif // #ifndef CS_GTL_SCRIPT_EXPORT_H_INCLUDED