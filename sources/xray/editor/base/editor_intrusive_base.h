////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_INTRUSIVE_BASE_H_INCLUDED
#define EDITOR_INTRUSIVE_BASE_H_INCLUDED

#include <xray/threading_policies.h>

namespace xray {
	namespace editor_base {

	class editor_intrusive_base {
	public:
		inline			editor_intrusive_base	( );

		template < typename T >
		inline	void	destroy			( T* object ) const;

		inline	u32		reference_count	( ) const { return m_reference_count; }

	private:
		friend class threading::mutex;
		friend class threading::single_threading_policy;
		u32		m_reference_count;
	}; // class editor_intrusive_base

	} // namespace editor_base
} // namespace xray

#include "editor_intrusive_base_inline.h"

#endif // #ifndef EDITOR_INTRUSIVE_BASE_H_INCLUDED