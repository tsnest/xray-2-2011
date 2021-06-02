////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export_macroses.h
//	Created 	: 03.07.2005
//  Modified 	: 10.04.2006
//	Author		: Dmitriy Iassenev
//	Description : script export macroses
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_SCRIPT_EXPORT_MACROSES_H_INCLUDED
#define CS_GTL_SCRIPT_EXPORT_MACROSES_H_INCLUDED

struct lua_State;

#ifdef CS_SCRIPT_EXPORTING
	namespace Loki {
		template <typename HeadType, typename TailType>
		struct Typelist {
			typedef HeadType	Head;
			typedef TailType	Tail;
		}; // struct Typelist

		struct NullType { };
	} // namespace Loki

#	define	CS_SCRIPT_EXPORT_TYPE_LIST		Loki::NullType
#	define	CS_TYPE_LIST_PUSH_FRONT(type)	typedef Loki::Typelist<type,CS_SCRIPT_EXPORT_TYPE_LIST> TypeList_##type;
#	define	CS_SAVE_TYPE_LIST(type)			TypeList_##type
#else // #ifdef CS_SCRIPT_EXPORTING
#	define	CS_SCRIPT_EXPORT_TYPE_LIST		
#	define	CS_TYPE_LIST_PUSH_FRONT(type)	;
#	define	CS_SAVE_TYPE_LIST(type)		
#endif // #ifdef CS_SCRIPT_EXPORTING

#define CS_DECLARE_SCRIPT_EXPORT_FUNCTION	public: static void script_export(lua_State* );

namespace gtl {

template <typename>
struct enum_script_exporter
{
	CS_DECLARE_SCRIPT_EXPORT_FUNCTION
};

template <typename>
struct class_script_exporter
{
	CS_DECLARE_SCRIPT_EXPORT_FUNCTION
};

}

#endif // #ifndef CS_GTL_SCRIPT_EXPORT_MACROSES_H_INCLUDED