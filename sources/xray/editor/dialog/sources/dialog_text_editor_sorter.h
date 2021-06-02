//-------------------------------------------------------------------------------------------
//	Created		: 04.02.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
//-------------------------------------------------------------------------------------------
#pragma once

namespace xray {
namespace dialog_editor {
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
} // namespace dialog_editor
} // namespace xray