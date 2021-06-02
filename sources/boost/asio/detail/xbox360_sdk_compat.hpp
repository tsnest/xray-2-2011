////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.01.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX360_SDK_COMPAT_HPP_INCLUDED
#define XBOX360_SDK_COMPAT_HPP_INCLUDED

//fake value of options that not exist in Xtl.h
#define AF_INET6        23              /* Internetwork Version 6 */
#define PF_INET6        AF_INET6

#define SO_SNDLOWAT     0x1003          /* send low-water mark */
#define SO_RCVLOWAT     0x1004          /* receive low-water mark */
#define SO_DEBUG        0x0001          /* turn on debugging info recording */
#define SO_KEEPALIVE    0x0008          /* keep connections alive */
#define SO_DONTROUTE    0x0010          /* just use interface addresses */

#define MSG_OOB         0x1             /* process out-of-band data */
#define MSG_PEEK        0x2             /* peek at incoming message */
#define MSG_DONTROUTE   0x4             /* send without using routing tables */


inline bool xbox360_check_option_support(int optname)
{
	switch (optname)
	{
	case SO_BROADCAST:
	case SO_DONTLINGER:
	case SO_EXCLUSIVEADDRUSE:
	case SO_LINGER:
	case SO_RCVBUF:
	case SO_REUSEADDR:
	case SO_SNDBUF:
	case SO_SNDTIMEO:
	case TCP_NODELAY:
		return true;

	};
	return false;
}


#endif //#ifndef XBOX360_SDK_COMPAT_HPP_INCLUDED