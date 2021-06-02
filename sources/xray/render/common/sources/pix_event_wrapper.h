////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PIX_EVENT_WRAPPER_H_INCLUDED
#define PIX_EVENT_WRAPPER_H_INCLUDED


#ifdef	DEBUG


#define PIX_EVENT(Name)	pix_event_wrapper	pixEvent##Name(L#Name)

class pix_event_wrapper
{
public:
		pix_event_wrapper(LPCWSTR wszName)	{	D3DPERF_BeginEvent	( D3DCOLOR_RGBA(127,0,0,255), wszName );}
		~pix_event_wrapper()				{	D3DPERF_EndEvent	();	}
};


#else	//	DEBUG

#define PIX_EVENT(Name)	{;}

#endif	//	DEBUG

#endif // #ifndef PIX_EVENT_WRAPPER_H_INCLUDED
