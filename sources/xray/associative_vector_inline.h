////////////////////////////////////////////////////////////////////////////
//	Created 	: 14.10.2005
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ASSOCIATIVE_VECTOR_INLINE_H_INCLUDED
#define XRAY_ASSOCIATIVE_VECTOR_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	\
	template <\
		typename key_type,\
		typename data_type,\
		template <typename T> class container_type,\
		typename compare_predicate_type\
	>

#define ASSOCIATIVE_VECTOR	\
	associative_vector<\
		key_type,\
		data_type,\
		container_type,\
		compare_predicate_type\
	>

TEMPLATE_SIGNATURE
inline ASSOCIATIVE_VECTOR::associative_vector												(key_compare const& predicate, allocator_type const& allocator) :
	value_compare		(predicate)
{
	XRAY_UNREFERENCED_PARAMETER		(allocator);
}

TEMPLATE_SIGNATURE
inline ASSOCIATIVE_VECTOR::associative_vector												(key_compare const& predicate) :
	value_compare		(predicate)
{
}

TEMPLATE_SIGNATURE
template <typename iterator_type>
inline ASSOCIATIVE_VECTOR::associative_vector												(iterator_type first, iterator_type last, key_compare const& predicate , allocator_type const& allocator ) :
	super			(first,last),
	value_compare		(predicate)
{
	std::sort			(begin(),end(),(value_compare&)(*this));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::iterator ASSOCIATIVE_VECTOR::begin						()
{
	actualize			();
	return				(super::begin());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::iterator ASSOCIATIVE_VECTOR::end						()
{
	actualize			();
	return				(super::end());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::const_iterator ASSOCIATIVE_VECTOR::begin				() const
{
	actualize			();
	return				(super::begin());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::const_iterator ASSOCIATIVE_VECTOR::end					() const
{
	actualize			();
	return				(super::end());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::reverse_iterator ASSOCIATIVE_VECTOR::rbegin				()
{
	actualize			();
	return				(super::rbegin());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::iterator ASSOCIATIVE_VECTOR::rend						()
{
	actualize			();
	return				(super::rend());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::const_reverse_iterator ASSOCIATIVE_VECTOR::rbegin		() const
{
	actualize			();
	return				(super::rbegin());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::const_reverse_iterator ASSOCIATIVE_VECTOR::rend			() const
{
	actualize			();
	return				(super::rend());
}

TEMPLATE_SIGNATURE
inline void ASSOCIATIVE_VECTOR::clear														()
{
	super::clear	();
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::size_type ASSOCIATIVE_VECTOR::max_size					() const
{
	return				(super::max_size());
}

TEMPLATE_SIGNATURE
inline u32 ASSOCIATIVE_VECTOR::size															() const
{
	return				(super::size());
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR::empty														() const
{
	return				(super::empty());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::allocator_type ASSOCIATIVE_VECTOR::get_allocator		() const
{
	return				(super::get_allocator());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::mapped_type &ASSOCIATIVE_VECTOR::operator[]				(key_type const& key)
{
	iterator			I = find(key);
	if (I != end())
		return			((*I).second);

	return				(insert(value_type(key,mapped_type())).first->second);
}

TEMPLATE_SIGNATURE
inline void ASSOCIATIVE_VECTOR::swap														(self_type &right)
{
	super::swap		(right);
}

TEMPLATE_SIGNATURE
inline void swap																			(ASSOCIATIVE_VECTOR &left, ASSOCIATIVE_VECTOR &right)
{
	left.swap			(right);
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::key_compare ASSOCIATIVE_VECTOR::key_comp				() const
{
	return				(key_compare());
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::value_compare ASSOCIATIVE_VECTOR::value_comp			() const
{
	return				(value_compare(key_comp()));
}

TEMPLATE_SIGNATURE
inline void ASSOCIATIVE_VECTOR::erase														(iterator element)
{
	super::erase	(element);
}

TEMPLATE_SIGNATURE
inline void ASSOCIATIVE_VECTOR::erase														(iterator first, iterator last)
{
	super::erase	(first,last);
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::size_type ASSOCIATIVE_VECTOR::erase						(key_type const& key)
{
	iterator			I = find(key);
	if (I == end())
		return			(0);

	erase				(I);
	return				(1);
}

TEMPLATE_SIGNATURE
inline void ASSOCIATIVE_VECTOR::actualize													() const
{
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::iterator ASSOCIATIVE_VECTOR::lower_bound				(key_type const& key)
{
	actualize			();
	value_compare		&self = *this;
	return				(std::lower_bound(begin(),end(),key,self));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::const_iterator ASSOCIATIVE_VECTOR::lower_bound			(key_type const& key) const
{
	actualize			();
	value_compare const&self = *this;
	return				(std::lower_bound(begin(),end(),key,self));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::iterator ASSOCIATIVE_VECTOR::upper_bound				(key_type const& key)
{
	actualize			();
	value_compare		&self = *this;
	return				(std::upper_bound(begin(),end(),key,self));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::const_iterator ASSOCIATIVE_VECTOR::upper_bound			(key_type const& key) const
{
	actualize			();
	value_compare const&self = *this;
	return				(std::upper_bound(begin(),end(),key,self));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::insert_result ASSOCIATIVE_VECTOR::insert				(value_type const& value)
{
	actualize			();
	bool				found = true;
	iterator			I = lower_bound(value.first);
	if (I == end() || operator()(value.first,(*I).first)) {
		I				= super::insert(I,value);
		found			= false;
	}
	else
		*I				= value;
	return				(insert_result(I,!found));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::iterator ASSOCIATIVE_VECTOR::insert						(iterator where, value_type const& value)
{
	if	(
			(where != end()) && 
			(operator()(*where,value)) &&
			((where - begin()) == size()) &&
			(!operator()(value,*(where + 1))) &&
			(operator()(*(where + 1),value))
		)
			return		(super::insert(where,value));

	return				(insert(val).first);
}

TEMPLATE_SIGNATURE
template <class iterator_type>
inline void ASSOCIATIVE_VECTOR::insert														(iterator_type first, iterator_type last)
{
	if ((last - first) < log2(size() + (last - first))) {
		for ( ; first != last; ++first)
			insert		(*first);

		return;
	}

	super::insert	(end(),first,last);
	std::sort			(begin(),end(),(value_compare&)(*this));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::iterator ASSOCIATIVE_VECTOR::find						(key_type const& key)
{
	actualize			();
	iterator			I = lower_bound(key);
	if (I == end())
		return			(end());

	if (operator()(key,(*I).first))
		return			(end());

	return				(I);
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::const_iterator ASSOCIATIVE_VECTOR::find					(key_type const& key) const
{
	actualize			();
	const_iterator		I = lower_bound(key);
	if (I == end())
		return			(end());

	if (operator()(key,(*I).first))
		return			(end());

	return				(I);
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::size_type ASSOCIATIVE_VECTOR::count						(key_type const& key) const
{
	actualize			();
	return				(find(key) == end() ? 0 : 1);
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::equal_range_result ASSOCIATIVE_VECTOR::equal_range		(key_type const& key)
{
	actualize			();
	iterator			I = lower_bound(key);
	if (I == end())
		return			(equal_range_result(end(),end()));

	if (operator()(key,(*I).first))
		return			(equal_range_result(I,I));

	ASSERT				(!operator()(key,(*I).first));
	return				(equal_range_result(I,I+1));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::const_equal_range_result ASSOCIATIVE_VECTOR::equal_range(key_type const& key) const
{
	actualize			();
	const_iterator		I = lower_bound(key);
	if (I == end())
		return			(const_equal_range_result(end(),end()));

	if (operator()(key,(*I).first))
		return			(const_equal_range_result(I,I));

	ASSERT				(!operator()(key,(*I).first));
	return				(const_equal_range_result(I,I+1));
}

TEMPLATE_SIGNATURE
inline typename ASSOCIATIVE_VECTOR::self_type &ASSOCIATIVE_VECTOR::operator=				(self_type const& right)
{
	(super&)(*this)	= right;
	return				(*this);
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR::operator<													(self_type const& right) const
{
	return				(((super const&)(*this)) < right);
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR::operator<=													(self_type const& right) const
{
	return				(!(right < left));
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR::operator>													(self_type const& right) const
{
	return				(right < left);
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR::operator>=													(self_type const& right) const
{
	return				(!(left < right));
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR::operator==													(self_type const& right) const
{
	return				(((super const&)(*this)) == right);
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR::operator!=													(self_type const& right) const
{
	return				(!(left == right));
}

#undef ASSOCIATIVE_VECTOR
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_ASSOCIATIVE_VECTOR_INLINE_H_INCLUDED