////////////////////////////////////////////////////////////////////////////
//	Created		: 01.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_THREADING_READER_WRITER_LOCK_H_INCLUDED
#define XRAY_THREADING_READER_WRITER_LOCK_H_INCLUDED

namespace xray {
namespace threading {

enum	lock_type_enum					{ lock_type_uninitialized, lock_type_read, lock_type_write };

// warning! reader_writer_lock is non-recursive!
class reader_writer_lock
{
public:
	class mutex_raii : private boost::noncopyable {
	public:
		mutex_raii	(reader_writer_lock const & lock, lock_type_enum	lock_type = lock_type_write) 
			: lock(lock), lock_type(lock_type) { lock.lock(lock_type); locked = true; }
		void		clear	() { if ( locked ) { lock.unlock(lock_type); locked = false; } }
		~mutex_raii	() { clear(); }
	private:
		reader_writer_lock const &	lock;
		lock_type_enum				lock_type;
		bool						locked;
	};
public:
			reader_writer_lock			() { m_readers_writers_counter.whole = 0; }
			~reader_writer_lock			();

	bool	try_lock_read				() const { return lock_read_impl(true); }
	void	lock_read					() const { lock_read_impl(false); }
	void	unlock_read					() const;

	bool	try_lock_write				() const { return lock_write_impl(true); }
	void	lock_write					() const { lock_write_impl(false); }
	void	unlock_write				() const;

	bool	try_lock					(lock_type_enum	lock_type) const;
	void	lock						(lock_type_enum	lock_type) const;
	void	unlock						(lock_type_enum	lock_type) const;

private:
	bool	lock_read_impl				(bool try_lock) const;
	bool	lock_write_impl				(bool try_lock) const;

private:
#pragma warning(push)
#pragma warning(disable:4201)
	union counters_type {
		struct {
			u16							readers_count;
			u16							writers_count;
			u32							writer_thread_id;
		};
		threading::atomic64_type		whole;
		counters_type					() : whole(0) {}
	};
#pragma warning(pop)

	mutable counters_type				m_readers_writers_counter;
};

typedef	reader_writer_lock::mutex_raii	reader_writer_lock_raii;

} // namespace threading
} // namespace xray

#endif // #ifndef XRAY_THREADING_READER_WRITER_LOCK_H_INCLUDED