////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_SELECTOR_ITEM_COMPARATOR_H_INCLUDED
#define EFFECT_SELECTOR_ITEM_COMPARATOR_H_INCLUDED

namespace xray 
{
	namespace editor 
	{

		ref class effect_selector_item_comparator : System::Collections::IComparer
		{
		public:
			effect_selector_item_comparator()
			{
				m_alphanum_comparer = gcnew controls::tree_node_alphanum_comparer();
			}

		public:
			virtual int Compare(Object^ x, Object^ y);

		private:
			controls::tree_node_alphanum_comparer^	m_alphanum_comparer;


		}; // class effect_selector_item_comparator


	} // namespace editor
} // namespace xray

#endif // #ifndef EFFECT_SELECTOR_ITEM_COMPARATOR_H_INCLUDED