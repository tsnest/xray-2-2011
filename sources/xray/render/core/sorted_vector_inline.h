////////////////////////////////////////////////////////////////////////////
//	Created		: 19.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace render_dx10 {

template< typename T, typename M>
sorted_vector< T, M >::~sorted_vector()
{
	for( iterator it = begin(); it< end(); ++it)
		DELETE( *it);
}

template< typename T, typename M>
T * sorted_vector< T, M >::try_add	( T const& element) 
{
	iterator it = std::lower_bound( begin(), end(), &element, sorted_vector_predicate);

	if ( it == end() || !(**it == element))
		return *insert( it, NEW(T)(element));

	//ASSERT(0, "The element with the specified name was already added.");
	return NULL;
}

template< typename T, typename M>
T & sorted_vector< T, M >::add	( M const& value) 
{
	iterator it = std::lower_bound( begin(), end(), value, sorted_vector_predicate);

	if ( it == end() || !(**it == value))
		return **insert( it, NEW(T)(value));
	else
		return **it;
}

template< typename T, typename M>
T * sorted_vector<T,M>::find	( M const& value) 
{
	iterator it = std::lower_bound( begin(), end(), value, sorted_vector_predicate);

	if ( it == end() || !(**it == value) )
		return NULL;
	else
		return *it;
}

} // namespace render
} // namespace xray
