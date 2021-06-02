////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_SINGLE_ITEM_H_INCLUDED
#define ANIMATION_SINGLE_ITEM_H_INCLUDED

#include "animation_item.h"

using namespace System;

namespace xray
{
	namespace animation
	{ 
		class animation_expression_emitter;
		typedef resources::resource_ptr < animation_expression_emitter, resources::unmanaged_intrusive_base > animation_expression_emitter_ptr;

		namespace mixing { class expression; }
	}
	namespace animation_editor
	{
		ref class animation_collections_editor;

		public ref class animation_single_item: animation_item
		{
		private:
			typedef xray::animation::mixing::expression expression;
			static animation_single_item	( );
		public:
			animation_single_item	( animation_collections_editor^ parent_editor, String^ animation_name );
		protected:
			~animation_single_item	( );
			
		public:
			static		String^					extension;

		public:
			[BrowsableAttribute(false)]
			property	Boolean			can_emit
			{
				virtual Boolean			get( ) override{ return is_loaded; }
			}

		public:
			void		load					( );
			virtual		expression	emit		( mutable_buffer& buffer, bool& is_last_animation ) override;

		private:
			void		item_loaded_callback	( xray::resources::queries_result& data );

		private:
			animation::animation_expression_emitter_ptr*		m_animation_ptr;		

		}; // class animation_single_item
	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_SINGLE_ITEM_H_INCLUDED