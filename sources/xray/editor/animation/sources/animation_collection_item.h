////////////////////////////////////////////////////////////////////////////
//	Created		: 10.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_COLLECTION_ITEM_H_INCLUDED
#define ANIMATION_COLLECTION_ITEM_H_INCLUDED

#include "animation_item.h"

using namespace System;
using namespace System::Collections::Generic;

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

		public enum class animation_collection_type
		{
			random,
			sequential
		}; // enum class animation_collection_type

		public ref class animation_collection_item: animation_item
		{
		private:
			typedef animation::animation_expression_emitter_ptr animation_expression_emitter_ptr;
			typedef animation::mixing::expression expression;
			static animation_collection_item	( );
		public:
			animation_collection_item			( animation_collections_editor^ parent_editor, String^ collection_name );
			
		public:
			static		String^						extension;

			[ReadOnlyAttribute(false)]
			property	String^						name  
			{
				String^		get( )	new				{ return m_name; }
				void		set( String^ value ) new{ m_name = value; }
			}
			[BrowsableAttribute(false)]
			property	List<animation_item^>^		items;
			property	Boolean						is_cyclic_repeat
			{
				Boolean		get( )					{ return m_is_cyclic_repeat; }
				void		set( Boolean value )	{ m_is_cyclic_repeat = value; }
			}
			property	Boolean						is_dont_repeat_previous
			{
				Boolean		get( )					{ return m_is_dont_repeat_previous; }
				void		set( Boolean value )	{ m_is_dont_repeat_previous = value; }
			}
			[BrowsableAttribute(false)]
			property	Boolean						is_internal_collection
			{
				Boolean		get( )					{ return m_is_internal_collection; }
				void		set( Boolean value )	{ m_is_internal_collection = value; }
			}
			property	animation_collection_type	collection_type
			{
				animation_collection_type get( ){ return m_type; }
				void set( animation_collection_type value ){ m_type = value; }
			}
			[BrowsableAttribute(false)]
			property	Boolean						can_emit
			{
				virtual Boolean	get( ) override{ return is_loaded && items->Count > 0; }
			}
			[BrowsableAttribute(false)]
			property	Boolean						is_correct
			{
						Boolean	get( );
			}

		public:
			Boolean		can_save				( );
			void		save					( );
			void		save					( configs::lua_config_value& config_value );
			void		load					( );
			void		load					( configs::lua_config_value const& config_value );
			void		add_item				( animation_item^ item );
			void		insert_item				( Int32 index, animation_item^ item );
			void		remove_item				( animation_item^ item );
			void		remove_item_at			( Int32 index );
			virtual		expression emit			( mutable_buffer& buffer, bool& is_last_animation ) override;

		private:
			void		load_anim_item			( configs::lua_config_value const& config_value, animation_collection_item^ item );
			void		save_anim_item			( configs::lua_config_value& config_value, animation_collection_item^ item );
			void		item_loaded_callback	( xray::resources::queries_result& data );
			void		sub_item_loaded_callback( Object^, EventArgs^ );

		private:
			Boolean								m_is_internal_collection;
			Boolean								m_is_cyclic_repeat;
			Boolean								m_is_dont_repeat_previous;
			animation_collection_type			m_type;
			Int32								m_loading_sub_items;
			Int32								m_current_animation_index;
			Boolean								m_is_child_last_animation;
			Random								m_random;

		}; // class animation_collection_item

	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_COLLECTION_ITEM_H_INCLUDED