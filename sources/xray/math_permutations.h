////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MATH_PERMUTATIONS_H_INCLUDED
#define MATH_PERMUTATIONS_H_INCLUDED

namespace xray {
namespace math {

void	first_permutation				(int * permutation, int n, int k);
bool	next_permutation				(int * permutation, int n, int k);

} // namespace math
} // namespace xray

#endif // #ifndef MATH_PERMUTATIONS_H_INCLUDED