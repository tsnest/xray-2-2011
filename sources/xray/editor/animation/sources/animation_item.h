////////////////////////////////////////////////////////////////////////////
//	Created		: 10.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_ITEM_H_INCLUDED
#define ANIMATION_ITEM_H_INCLUDED

#include "animation_item_type.h"

using namespace System;
using namespace System::ComponentModel;

namespace xray
{
	namespace animation { namespace mixing { class expression; } }
	namespace animation_editor
	{
		ref class animation_collections_editor;

		public ref class animation_item abstract
		{
		private:
			typedef xray::animation::mixing::expression expression;
		public:
			animation_item	( animation_collections_editor^	parent_editor );

		public:
			[ReadOnlyAttribute(true)]
			property	String^					name
			{
				String^ get( )					{ return m_name; }
				void	set( String^ value )	{ m_name = value; }
			}
			property	animation_item_type		type
			{
				animation_item_type		get		( );
			}

			[BrowsableAttribute(false)]
			property	Boolean					is_loaded
			{
				Boolean					get		( ){ return m_is_loaded; }
			}
			[BrowsableAttribute(false)]
			property	animation_collections_editor^	parent_editor
			{
				animation_collections_editor^	get( ){ return m_parent_editor; }
			}
			[BrowsableAttribute(false)]
			property	Boolean			can_emit
			{
				virtual Boolean			get( ) abstract;
			}

			event		EventHandler^			loaded;
			
		public:
			virtual		expression				emit	( mutable_buffer& buffer, bool& is_last_animation ) abstract;

		protected:
			String^								m_name;
			Boolean								m_is_loaded;
			animation_collections_editor^		m_parent_editor;

		}; // class animation_item

	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_ITEM_H_INCLUDED