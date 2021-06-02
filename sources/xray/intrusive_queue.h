////////////////////////////////////////////////////////////////////////////
//	Created 	: 08.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_LIST_FAST_H_INCLUDED
#define XRAY_INTRUSIVE_LIST_FAST_H_INCLUDED

#include <xray/threading_policies.h>

namespace xray {

template <typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext, typename Policy = threading::mutex>
class XRAY_CORE_API intrusive_queue : private core::noncopyable {
public:
	typedef				Policy				policy;
	typedef				T					data_type;

public:
	inline				intrusive_queue	();

	inline	void		push_back			(T* object);

	inline	T*			pop_front			();
	inline	T*			front				();
	inline	bool		empty				() const;

	static T*			get_next_of_object	(T const * object) { return object->*MemberNext; }

	Policy &			threading_policy	() const { return m_threading_policy; }

private:
	T * 				m_pop_list;
	char				m_cache_line_pad	[XRAY_MAX_CACHE_LINE_SIZE - sizeof(T*) ];
	T *					m_push_list;

	mutable Policy		m_threading_policy;
};

} // namespace xray

#include <xray/intrusive_queue_inline.h>

#endif // #ifndef XRAY_INTRUSIVE_LIST_FAST_H_INCLUDED