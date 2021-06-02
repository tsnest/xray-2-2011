////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_CALLBACK_BINDERS_H_INCLUDED
#define MATERIAL_CALLBACK_BINDERS_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor
	{
		ref class material_property_base; 

		delegate void material_effect_fs_iterator_delegate( material_property_base^, xray::vfs::vfs_locked_iterator const & );

		ref class material_effect_fs_iterator_binder
		{
		public:
			material_effect_fs_iterator_binder( material_property_base^ prop, material_effect_fs_iterator_delegate^ callback )
			{
				m_callback		= callback;
				m_prop			= prop;
			}

			material_property_base^					m_prop;
			material_effect_fs_iterator_delegate^	m_callback;

			void func( xray::vfs::vfs_locked_iterator const & fs_it )
			{
				m_callback( m_prop, fs_it );
			}		
		};

	} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_CALLBACK_BINDERS_H_INCLUDED