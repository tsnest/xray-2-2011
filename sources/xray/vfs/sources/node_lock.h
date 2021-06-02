////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_NODE_LOCK_H_INCLUDED
#define VFS_NODE_LOCK_H_INCLUDED

#include <xray/threading_reader_writer_lock.h>
#include <xray/vfs/types.h>

namespace xray {
namespace vfs {

class vfs_reader_writer_lock
{
public:
			vfs_reader_writer_lock		() { m_counters.whole = 0; }
			~vfs_reader_writer_lock		();

	bool	try_lock_read				() const { return lock(lock_type_read, lock_operation_try_lock); }
	bool	try_lock_soft_read			() const { return lock(lock_type_soft_read, lock_operation_try_lock); }
	bool	try_lock_write				() const { return lock(lock_type_write, lock_operation_try_lock); }
	bool	try_lock_soft_write			() const { return lock(lock_type_soft_write, lock_operation_try_lock); }
	bool	try_lock					(lock_type_enum lock_type) const;

	void	lock_read					() const { lock_read_impl(lock_operation_lock); }
	void	lock_soft_read				() const { lock_soft_read_impl(lock_operation_lock); }
	void	lock_write					() const { lock_write_impl(lock_operation_lock); }
	void	lock_soft_write				() const { lock_soft_write_impl(lock_operation_lock); }
	bool	lock						(lock_type_enum lock_type, lock_operation_enum operation = lock_operation_lock) const;

	void	unlock_read					() const;
	void	unlock_soft_read			() const;
	void	unlock_write				() const;
	void	unlock_soft_write			() const;
	void	unlock						(lock_type_enum lock_type) const;
	
	bool	upgrade						(lock_type_enum from_lock, lock_type_enum to_lock, lock_operation_enum operation) const;

	bool	has_some_lock				() const { return m_counters.whole != 0; }
	bool	has_lock					(lock_type_enum lock_type) const;

private:
	bool	lock_read_impl				(lock_operation_enum operation) const;
	bool	lock_soft_read_impl			(lock_operation_enum operation) const;
	bool	lock_write_impl				(lock_operation_enum operation) const;
	bool	lock_soft_write_impl		(lock_operation_enum operation) const;

private:
#pragma warning(push)
#pragma warning(disable:4201)
	union counters_type
	{
		enum { max_soft_readers_count	=	(1 << 14) - 1,
			   max_readers_count		=	(1 << 10) - 1,
			   max_soft_writers_count	=	(1 << 6)  - 1,
			   max_writers_count		=	(1 << 1)  - 1,	};

		struct {
			u32							soft_readers_count	: 14;
			u32							readers_count		: 10;
			u32							soft_writers_count	: 6;
			u32							writers_count		: 1;
		};
		threading::atomic32_type		whole;
		counters_type					() : whole(0) {}

		void	change_unsafe			(int change, lock_type_enum lock_type);
	};
#pragma warning(pop)

	mutable counters_type				m_counters;
};

class vfs_reader_writer_lock_raii
{
public:
	vfs_reader_writer_lock_raii			(vfs_reader_writer_lock	& lock, lock_type_enum lock_type) 
										: m_lock(& lock), m_lock_type(lock_type) { m_lock->lock(m_lock_type); }
	~vfs_reader_writer_lock_raii		() { m_lock->unlock(m_lock_type); }

private:
	vfs_reader_writer_lock	*			m_lock;
	lock_type_enum						m_lock_type;
};

threading::lock_type_enum	to_threading_lock_type	(lock_type_enum lock_type);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_NODE_LOCK_H_INCLUDED