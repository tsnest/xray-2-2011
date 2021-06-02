////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_COLLECTION_CONTEXTS_MANAGER_H_INCLUDED
#define ANIMATION_COLLECTION_CONTEXTS_MANAGER_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;

namespace xray 
{
	namespace animation_editor 
	{
		ref class animation_collections_editor;

		ref class animation_collection_contexts_manager: public editor_base::i_context_manager 
		{
		public:
			animation_collection_contexts_manager			( animation_collections_editor^ editor ):m_editor( editor ) {};
			virtual bool			context_fit				( String^ context );
			virtual List<String^>^	get_contexts_list		( );

		private:
			animation_collections_editor^ m_editor;
		}; // class animation_collection_contexts_manager

	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_COLLECTION_CONTEXTS_MANAGER_H_INCLUDED