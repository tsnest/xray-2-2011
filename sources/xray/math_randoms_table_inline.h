#ifndef RANDOMS_RANGE_INLINE_H_INCLUDED
#define RANDOMS_RANGE_INLINE_H_INCLUDED

namespace xray {
namespace math {

template <u32 num_randoms, u32 range, allow_duplicates_bool allow_duplicates>
randoms_table<num_randoms, range, allow_duplicates>::randoms_table (u32 seed)
{
	random32				random(seed);

#ifndef MASTER_GOLD
	if ( !identity(allow_duplicates) )
		R_ASSERT			(num_randoms <= 32768, "maximum table length without duplicates is 32768");
#endif // #ifndef MASTER_GOLD
	
	for ( u32 i=0; i<num_randoms; ++i )
	{
		u32 number		=	0;
		while ( xray::identity(true) ) 	
		{
			number		=	random.random(range);
			if ( identity(allow_duplicates == allow_duplicates_true) || 
				 std::find(m_randoms.begin(), m_randoms.end(), number) == m_randoms.end() )
			{
				break;
			}
		}

		m_randoms.push_back		(number);
	}

	m_index				=	0;
}

template <u32 num_randoms, u32 range, allow_duplicates_bool allow_duplicates>
u32   randoms_table<num_randoms, range, allow_duplicates>::next_random ()
{
	u32	result			=	m_randoms[m_index];
	++m_index;
	if ( m_index == num_randoms )
	{
		m_index			=	0;
	}

	return					result;
}

template <u32 num_randoms, u32 range, allow_duplicates_bool allow_duplicates>
void   randoms_table<num_randoms, range, allow_duplicates>::to_first_random	()
{
	m_index				=	0;
}

} // namespace math
} // namespace xray

#endif // #define RANDOMS_RANGE_INLINE_H_INCLUDED