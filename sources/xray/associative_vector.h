////////////////////////////////////////////////////////////////////////////
//	Created 	: 14.10.2005
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ASSOCIATIVE_VECTOR_H_INCLUDED
#define XRAY_ASSOCIATIVE_VECTOR_H_INCLUDED

#include <algorithm>										// for std::lower_bound
#include <xray/associative_vector_compare_predicate.h>		// for associative_vector_compare_predicate

template <
	typename key_type,
	typename data_type,
	template <typename T> class container_type,
	typename compare_predicate_type = std::less< key_type >
>
class associative_vector : 
	protected
		container_type<
			std::pair<
				key_type,
				data_type
			>
		>,
	protected
		associative_vector_compare_predicate<
			key_type,
			data_type,
			compare_predicate_type
		>
{
private:
	typedef 
		associative_vector<
			key_type,
			data_type,
			container_type,
			compare_predicate_type
		>													self_type;

	typedef 
		container_type<
			std::pair<
				key_type,
				data_type
			>
		>													super;

public:
	typedef 
		associative_vector_compare_predicate<
			key_type,
			data_type,
			compare_predicate_type
		>													value_compare;

public:
	typedef typename super::allocator_type					allocator_type;
	typedef typename super::const_pointer					const_pointer;
	typedef typename super::const_reference					const_reference;
	typedef typename super::const_iterator					const_iterator;
	typedef typename super::const_reverse_iterator			const_reverse_iterator;
	typedef typename super::pointer							pointer;
	typedef typename super::reference						reference;
	typedef typename super::iterator						iterator;
	typedef typename super::reverse_iterator				reverse_iterator;
	typedef typename allocator_type::difference_type		difference_type;
	typedef compare_predicate_type							key_compare;
	typedef data_type										mapped_type;
	typedef typename super::size_type						size_type;
	typedef typename super::value_type						value_type;
	typedef std::pair<iterator,bool>						insert_result;
	typedef std::pair<iterator,iterator>					equal_range_result;
	typedef std::pair<const_iterator,const_iterator>		const_equal_range_result;

private:
	inline	void						actualize			() const;

public:
	template <typename iterator_type>
	inline								associative_vector	(iterator_type first, iterator_type last, key_compare const& predicate = key_compare(), allocator_type const& allocator = allocator_type());
	inline								associative_vector	(key_compare const& predicate = key_compare(), allocator_type const& allocator = allocator_type());
	inline					explicit	associative_vector	(key_compare const& predicate);
	inline	iterator					begin				();
	inline	iterator					end					();
	inline	reverse_iterator			rbegin				();
	inline	iterator					rend				();
	inline	insert_result				insert				(value_type const& value);
	inline	iterator					insert				(iterator where, value_type const& value);
	template <class iterator_type>
	inline	void						insert				(iterator_type first, iterator_type last);
	inline	void						erase				(iterator element);
	inline	void						erase				(iterator first, iterator last);
	inline	size_type					erase				(key_type const& key);
	inline	void						clear				();
	inline	iterator					find				(key_type const& key);
	inline	iterator					lower_bound			(key_type const& key);
	inline	iterator					upper_bound			(key_type const& key);
	inline	equal_range_result			equal_range			(key_type const& key);
	inline	void						swap				(self_type& object);

public:
	inline	const_iterator				begin				() const;
	inline	const_iterator				end					() const;
	inline	const_reverse_iterator		rbegin				() const;
	inline	const_reverse_iterator		rend				() const;
	inline	const_iterator				find				(key_type const& key) const;
	inline	const_iterator				lower_bound			(key_type const& key) const;
	inline	const_iterator				upper_bound			(key_type const& key) const;
	inline	const_equal_range_result	equal_range			(key_type const& key) const;
	inline	size_type					count				(key_type const& key) const;
	inline	size_type					max_size			() const;
	inline	u32							size				() const;
	inline	bool						empty				() const;
	inline	key_compare					key_comp			() const;
	inline	value_compare				value_comp			() const;
	inline	allocator_type				get_allocator		() const;

public:
	inline	mapped_type					&operator[]			(key_type const& key);
	inline	self_type					&operator=			(self_type const& right);
	inline	bool						operator<			(self_type const& right) const;
	inline	bool						operator<=			(self_type const& right) const;
	inline	bool						operator>			(self_type const& right) const;
	inline	bool						operator>=			(self_type const& right) const;
	inline	bool						operator==			(self_type const& right) const;
	inline	bool						operator!=			(self_type const& right) const;
};

template <
	typename key_type,
	typename data_type,
	template <typename T> class container_type,
	typename compare_predicate_type
>
inline		void						swap				(
				associative_vector<
					key_type,
					data_type,
					container_type,
					compare_predicate_type
				>	&left,
				associative_vector<
					key_type,
					data_type,
					container_type,
					compare_predicate_type
				>	&right
			);

#include <xray/associative_vector_inline.h>

#endif // #ifndef XRAY_ASSOCIATIVE_VECTOR_H_INCLUDED