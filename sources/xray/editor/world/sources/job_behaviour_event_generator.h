////////////////////////////////////////////////////////////////////////////
//	Created		: 01.06.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_DEFAULT_TIMER_H_INCLUDED
#define JOB_BEHAVIOUR_DEFAULT_TIMER_H_INCLUDED

#include "job_behaviour_base.h"
#include "attribute.h"

namespace xray {
namespace editor {

ref class object_job;

public ref class job_behaviour_event_generator : job_behaviour_base {

typedef xray::editor::wpf_controls::property_container				wpf_property_container;

public:
	job_behaviour_event_generator( object_job^ parent_job ):job_behaviour_base(parent_job){ };

}; // class job_behaviour_event_generator

} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_DEFAULT_TIMER_H_INCLUDED