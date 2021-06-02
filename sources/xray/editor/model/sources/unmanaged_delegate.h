////////////////////////////////////////////////////////////////////////////
//	Created		: 31.10.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef UNMANAGED_DELEGATE_H_INCLUDED
#define UNMANAGED_DELEGATE_H_INCLUDED

namespace xray {
namespace model_editor {
template<typename T, typename Arg>
ref class unmanaged_delegate {
public:
	typedef void (T::*MethodType)( Arg, bool );
	unmanaged_delegate( T* object, MethodType method, Arg arg )
		:m_object( object ), m_method( method ), m_arg( arg ){}
	void invoke( bool param )
	{
		(m_object->*m_method)( m_arg, param );
	}
private:
	T*				m_object;
	MethodType		m_method;
	Arg				m_arg;			

}; // class unmanaged_delegate

} // namespace model_editor
} // namespace xray

#endif // #ifndef UNMANAGED_DELEGATE_H_INCLUDED