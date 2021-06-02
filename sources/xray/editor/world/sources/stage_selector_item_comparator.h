////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SELECTOR_ITEM_COMPARATOR_H_INCLUDED
#define STAGE_SELECTOR_ITEM_COMPARATOR_H_INCLUDED

namespace xray
{
	namespace editor
	{
		ref class stage_selector_item_comparator: System::Collections::IComparer
		{
		public:
			virtual int Compare(Object^ x, Object^ y);

		}; // class stage_selector_item_comparator
	} // namespace editor
} // namespace xray

#endif // #ifndef STAGE_SELECTOR_ITEM_COMPARATOR_H_INCLUDED