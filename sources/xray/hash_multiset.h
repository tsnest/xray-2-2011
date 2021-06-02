#ifndef HASH_MULTISET_H_INCLUDED
#define HASH_MULTISET_H_INCLUDED

#include <xray/threading_policies.h>

namespace xray {
namespace detail
{
	template <int Size>
	class fixed_size_policy {
	public:
		enum { fixed_size = Size };
	};

	template <class Value>
	struct null_hash  { int	 operator () (Value &) { return 0; } };

	template <class Value>
	struct null_equal { bool operator () (Value const &, Value const &) { return 0; } };

} // namespace detail

template <class Derived>
class hash_multiset_intrusive_base;

//-----------------------------------------------------------------------------------
// Value must inherit from hash_multiset_intrusive_base<Value>
//-----------------------------------------------------------------------------------
template <	class	Value,
			class	MemberNextType,
			MemberNextType	Value::*MemberNext,
			class   SizePolicy								, // can be fixed_size_policy
			class	HashFunc	=	detail::null_hash<Value>,
			class	EqualKey	=	detail::null_equal<Value>,
			class   ThreadingPolicy = threading::single_threading_policy
		 >
class hash_multiset : private boost::noncopyable {
public:
	//-----------------------------------------------------------------------------------
	// public typedefs
	//-----------------------------------------------------------------------------------
	typedef					Value					key_type;
	typedef					Value					value_type;
	typedef					HashFunc				hasher;
	typedef					EqualKey				key_equal;

	typedef					size_t					size_type;
	typedef					ptrdiff_t				difference_type;
	typedef					hash_multiset*			pointer;
	typedef					const hash_multiset*	const_pointer;
	typedef					hash_multiset&			reference;
	typedef					const hash_multiset&	const_reference;

	class					iterator;
	class					reverse_iterator;
	typedef					iterator				const_iterator;
	typedef					reverse_iterator		const_reverse_iterator;


public:
							hash_multiset			();
						   ~hash_multiset			();

    //-----------------------------------------------------------------------------------
    // operations
    //-----------------------------------------------------------------------------------
	iterator				find					(Value* const value);
	iterator				find					(u32 hash);
	const_iterator			find					(Value* const value) const {  return const_cast<hash_multiset*>(this)->find(value); }
	const_iterator			find					(u32 hash) const {  return const_cast<hash_multiset*>(this)->find(hash); }
	void					insert					(Value* value);
	void					insert					(u32 hash, Value * value);
	template <class ShouldOverlap>
	void					insert					(u32 hash, Value * value, ShouldOverlap const & should_overlap_predicate);
	Value *					erase					(const iterator & it);
	void					erase					(Value * value);
	void					erase					(u32 hash, Value * value);
	template <class Deleter>
	void					clear					(Deleter const& deleter);
	void					clear					();

	static Value *			get_next_of_object		(Value * const object) { return object->*MemberNext; }
	static void				set_next_of_object		(Value * const object, Value * const value) { object->*MemberNext = value; }

	//-----------------------------------------------------------------------------------
	// accessors & iterators
	//-----------------------------------------------------------------------------------
	u32						size					() const { return m_size; }
	bool					empty					() const { return !m_size; }

	iterator				begin					();
	iterator				end						();
	const_iterator			cbegin					() const { return const_cast<hash_multiset*>(this)->begin();	 }
	const_iterator			cend					() const { return const_cast<hash_multiset*>(this)->end();	 }
	
	reverse_iterator		rbegin					();
	reverse_iterator		rend					();
	const_reverse_iterator	rbegin					() const { return const_cast<hash_multiset*>(this)->rbegin(); }
	const_reverse_iterator	rend					() const { return const_cast<hash_multiset*>(this)->rend();	 }

	//-----------------------------------------------------------------------------------
	// misc
	//-----------------------------------------------------------------------------------
	hasher					hash_funct				() const { return m_hash;; }
	key_equal				key_eq					() const { return m_equal_key; }
 
private:
	ThreadingPolicy			m_threading_policy;
	key_equal				m_equal_key;
	hasher					m_hash;
	value_type*				m_buffer				[SizePolicy::fixed_size];
	u32						m_size;
};

} // namespace xray

#include <xray/hash_multiset_inline.h>

#endif // HASH_MULTISET_H_INCLUDED