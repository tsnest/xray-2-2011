////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/math_permutations.h>

namespace xray {
namespace math {

void	first_permutation				(int * permutation, int const n, int const k)
{
	XRAY_UNREFERENCED_PARAMETER				(n);
	for ( int i=0; i<k; ++i )
		permutation[i]					=	i + 1;
}

bool	next_permutation				(int * permutation, int const n, int const k)
{
	u32 const taken_size				=	(n + 1) * sizeof(bool);
	bool * const taken					=	(bool *)ALLOCA(taken_size);
	memory::zero							(taken, taken_size);

	for ( int i=0; i<k; ++i )
		taken[ permutation[i] ]			=	true;

	for ( int i=k-1; i>=0; --i )
	{
		int	j							=	0;
		
		for ( j=permutation[i] + 1; j <= n; ++j )
			if ( !taken[j] )
				break;
		
		if ( j > n )
		{
			taken[permutation[i]]		=	false;
			continue;
		}

		taken[ permutation[i] ]			=	false;
		taken[j]						=	true;
		permutation[i]					=	j;

		for ( i=i+1; i<k; ++i )
		{
			for ( int j=1; j<=n; ++j )
				if ( !taken[j] )
				{
					taken[j]			=	true;
					permutation[i]		=	j;
					break;
				}
		}

		return								true;
	}

	return									false;
}

} // namespace math
} // namespace xray