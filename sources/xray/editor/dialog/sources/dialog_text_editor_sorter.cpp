//-------------------------------------------------------------------------------------------
//	Created		: 04.02.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
//-------------------------------------------------------------------------------------------
/*
 * The Alphanum Algorithm is an improved sorting algorithm for strings
 * containing numbers.  Instead of sorting numbers in ASCII order like
 * a standard sort, this algorithm sorts numbers in numeric order.
 *
 * The Alphanum Algorithm is discussed at http://www.DaveKoelle.com
 *
 * Based on the Java implementation of Dave Koelle's Alphanum algorithm.
 * Contributed by Jonathan Ruckwood <jonathan.ruckwood@gmail.com>
 *
 * Adapted by Dominik Hurnaus <dominik.hurnaus@gmail.com> to
 *   - correctly sort words where one word starts with another word
 *   - have slightly better performance
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/*
 * Please compare against the latest Java version at http://www.DaveKoelle.com
 * to see the most recent modifications
 */

#include "pch.h"
#include "dialog_text_editor_sorter.h"

using xray::dialog_editor::AlphanumComparator;
using xray::dialog_editor::ChunkType;
using namespace System;
using namespace System::Collections;
using namespace System::Text;

bool AlphanumComparator::InChunk(char ch, char otherCh)
{
	ChunkType type = ChunkType::Alphanumeric;

	if(isdigit(otherCh))
		type = ChunkType::Numeric;

	if((type==ChunkType::Alphanumeric && isdigit(ch))
		|| (type==ChunkType::Numeric && !isdigit(ch)))
			return false;

	return true;
}

int AlphanumComparator::Compare(String^ x, String^ y)
{
	String^ s1 = x;
	String^ s2 = y;
	if(s1==nullptr || s2==nullptr)
		return 0;


	int thisMarker = 0, thisNumericChunk = 0;
	int thatMarker = 0, thatNumericChunk = 0;

	while((thisMarker<s1->Length) || (thatMarker<s2->Length))
	{
		if(thisMarker>=s1->Length)
			return -1;
		else if(thatMarker>=s2->Length)
			return 1;

		char thisCh = (char)s1[thisMarker];
		char thatCh = (char)s2[thatMarker];

		StringBuilder^ thisChunk = gcnew StringBuilder();
		StringBuilder^ thatChunk = gcnew StringBuilder();

		while((thisMarker<s1->Length) && (thisChunk->Length==0 || InChunk(thisCh, (char)thisChunk[0])))
		{
			thisChunk->Append(thisCh);
			thisMarker++;

			if(thisMarker<s1->Length)
				thisCh = (char)s1[thisMarker];
		}

		while((thatMarker<s2->Length) && (thatChunk->Length==0 || InChunk(thatCh, (char)thatChunk[0])))
		{
			thatChunk->Append(thatCh);
			thatMarker++;

			if(thatMarker<s2->Length)
				thatCh = (char)s2[thatMarker];
		}

		int result = 0;
		// If both chunks contain numeric characters, sort them numerically
		if(isdigit(thisChunk[0]) && isdigit(thatChunk[0]))
		{
			thisNumericChunk = Convert::ToInt32(thisChunk->ToString());
			thatNumericChunk = Convert::ToInt32(thatChunk->ToString());

			if(thisNumericChunk<thatNumericChunk)
				result = -1;

			if(thisNumericChunk>thatNumericChunk)
				result = 1;
		}
		else
			result = thisChunk->ToString()->CompareTo(thatChunk->ToString());

		if(result!=0)
			return result;
	}
	return 0;
}
