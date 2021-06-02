////////////////////////////////////////////////////////////////////////////
//	Created		: 06.12.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_JOB_RESOURCE_H_INCLUDED
#define I_JOB_RESOURCE_H_INCLUDED

namespace xray {
	namespace editor {
	ref class object_job;
public interface class	i_job_resource {
public:
	void				setup_job			( object_job^ job ) = 0;
	
}; // class i_job_resource


} // namespace editor
} // namespace xray

#endif // #ifndef I_JOB_RESOURCE_H_INCLUDED