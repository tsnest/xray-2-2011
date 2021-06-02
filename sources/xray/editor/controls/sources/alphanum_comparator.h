////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ALPHANUM_COMPARATOR_H_INCLUDED
#define ALPHANUM_COMPARATOR_H_INCLUDED

namespace xray {
namespace editor {
namespace controls {
	public enum class ChunkType: int 
	{
		Alphanumeric = 0,
		Numeric = 1,
	};

	using namespace System;

	public ref class AlphanumComparator: Collections::Generic::IComparer<String^>
	{
		private:
					bool	InChunk		(char ch, char otherCh);

		public:
			virtual	int		Compare		(System::String^ x, System::String^ y);
	}; // ref class AlphanumComparator

} // namespace controls
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef ALPHANUM_COMPARATOR_H_INCLUDED