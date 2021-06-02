////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TREE_NODE_ALPHANUM_COMPARER_H_INCLUDED
#define TREE_NODE_ALPHANUM_COMPARER_H_INCLUDED

typedef System::Collections::IComparer CIComparer;

namespace xray {
	namespace editor {
		namespace controls {

			ref class AlphanumComparator;

			public ref class tree_node_alphanum_comparer : CIComparer
			{
			public:
				tree_node_alphanum_comparer()
				{
					in_constructor();
				}

			private:
				void in_constructor();

			private:
				AlphanumComparator^ m_node_comparer;

			public:
				virtual int Compare(Object^ x, Object^y);

			}; // class tree_node_alphanum_comparer

		} // namespace controls
	} // namespace editor
} // namespace xray

#endif // #ifndef TREE_NODE_ALPHANUM_COMPARER_H_INCLUDED