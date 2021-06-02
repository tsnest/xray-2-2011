////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TREE_VIEW_SELECTION_EVENT_ARGS_H_INCLUDED
#define TREE_VIEW_SELECTION_EVENT_ARGS_H_INCLUDED



using namespace System;
using namespace System::Collections::Generic;

namespace xray {
	namespace editor {
		namespace controls {

			ref class tree_node;

			public ref class tree_view_selection_event_args: EventArgs
			{
			public:
				tree_view_selection_event_args( List<tree_node^>^ old_selection, List<tree_node^>^ new_selection )
				{
					this->old_selection = old_selection;
					this->new_selection = new_selection;
				}

			List<tree_node^>^ old_selection;
			List<tree_node^>^ new_selection;

			}; // class tree_view_selection_event_args

		} // namespace controls
	} // namespace editor
} // namespace xray

#endif // #ifndef TREE_VIEW_SELECTION_EVENT_ARGS_H_INCLUDED