////////////////////////////////////////////////////////////////////////////
//	Created		: 10.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_RESOURCE_RECONSTRUCTION_INFO_H_INCLUDED
#define RESOURCES_RESOURCE_RECONSTRUCTION_INFO_H_INCLUDED

namespace xray {
namespace resources {

class XRAY_CORE_API resource_reconstruction_info 
{
public:
					resource_reconstruction_info	();
	void			update_reconstruction_info		(u64 update_tick);
	u32				reconstruction_size				() const;
	//u64				last_used_tick					() 

private:
	u64				m_reconstruction_info_actuality_tick;
	u32				m_reconstruction_size;

}; // class resources_resource_reconstruction_info

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_RESOURCE_RECONSTRUCTION_INFO_H_INCLUDED