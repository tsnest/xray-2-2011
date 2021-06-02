////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_EDITOR_BINDERS_H_INCLUDED
#define TEXTURE_EDITOR_BINDERS_H_INCLUDED

namespace xray 
{
	namespace editor 
	{

		generic<class TType>
		ref class simple_binder
		{
		public:
			simple_binder( TType data, System::Action<TType>^ callback )
			{
				m_data		= data;
				m_callback	= callback;
			}

			TType				m_data;
			System::Action<TType>^		m_callback;

			void func( )
			{
				m_callback( m_data );
			}

		}; // class texture_editor_binders

		generic<class TType>
		delegate void resource_binder_delegate( TType, xray::resources::queries_result& result );

		generic<class TType>
		ref class resource_binder
		{
		public:
			resource_binder( TType data, resource_binder_delegate<TType>^ callback )
			{
				m_data		= data;
				m_callback	= callback;
			}

			TType									m_data;
			resource_binder_delegate<TType>^		m_callback;

			void func( xray::resources::queries_result& result )
			{
				m_callback( m_data, result );
			}

		}; // class texture_editor_binders

	} // namespace editor
} // namespace xray

#endif // #ifndef TEXTURE_EDITOR_BINDERS_H_INCLUDED