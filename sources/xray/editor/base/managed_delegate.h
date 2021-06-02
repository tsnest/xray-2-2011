////////////////////////////////////////////////////////////////////////////
//	Created		: 29.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MANAGED_DELEGATE_H_INCLUDED
#define MANAGED_DELEGATE_H_INCLUDED

namespace xray {
namespace vfs { struct vfs_notification; }
} // namespace xray

delegate void rqDelegate(xray::resources::queries_result& data);
delegate void grDelegate(xray::resources::unmanaged_resource_ptr data);
delegate void fsDelegate(xray::vfs::vfs_locked_iterator const & data);
delegate void fwDelegate(xray::vfs::vfs_notification const& info);


template <class T, class D> struct qr
{
	typedef D Delegate;

	qr(D^ dd, xray::memory::base_allocator*	a):m_delegate(dd), m_allocator(a), m_rejected(false){}
	void callback(T data)
	{
		if(!m_rejected)
		{
			D^ d			= m_delegate;
			d				(data);
		}else
		{
			LOG_WARNING("rejected query - skipping");
		}
		XRAY_DELETE_IMPL(m_allocator, this);
	}
	xray::memory::base_allocator*	m_allocator;
	gcroot<D^>						m_delegate;
	bool							m_rejected;
};

template <class T, class D> struct qr2
{
	typedef D Delegate;

	qr2(D^ dd):m_delegate(dd){}
	void callback(T data)
	{
		D^ d	= m_delegate;
		d				(data);
//		DELETE			(this); // WARNING!!! You must delete this struct manually!
	}
	gcroot<D^>	m_delegate;
};

typedef qr<xray::resources::queries_result&, rqDelegate>		query_result_delegate;
typedef qr<xray::vfs::vfs_locked_iterator const&, fsDelegate>	fs_iterator_delegate;
typedef qr2<xray::vfs::vfs_notification const&, fwDelegate>		file_watcher_delegate;

#endif // #ifndef MANAGED_DELEGATE_H_INCLUDED