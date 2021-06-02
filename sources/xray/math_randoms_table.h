#ifndef RANDOMS_RANGE_H_INCLUDED
#define RANDOMS_RANGE_H_INCLUDED

#include <xray/math_randoms_generator.h>

namespace xray {
namespace math {

enum allow_duplicates_bool { allow_duplicates_false, allow_duplicates_true };

template <u32 num_randoms, u32 range = u32(-1), allow_duplicates_bool allow_duplicates = allow_duplicates_true>
class randoms_table
{
public:
			// generated numbers [0..range-1]
			randoms_table			(u32 seed = 0);
	u32		next_random				();
	void	to_first_random			();

private:
	u32								m_index;
	fixed_vector<u32, num_randoms>	m_randoms;
};

} // namespace math
} // namespace xray

#include <xray/math_randoms_table_inline.h>

#endif // #define RANDOMS_RANGE_H_INCLUDED