////////////////////////////////////////////////////////////////////////////
//	Created 	: 05.02.2008
//	Author		: Konstantin Slipchenko
//	Description : element collision interface
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_CONTACT_JOINT_INFO_H_INCLUDED
#define XRAY_CONTACT_JOINT_INFO_H_INCLUDED

#include <xray/collision/contact_info.h>

struct contact_joint_info
{
	xray::collision::contact_info		c;
	float								integration_step;
}; // struct contact_joint_info

#endif