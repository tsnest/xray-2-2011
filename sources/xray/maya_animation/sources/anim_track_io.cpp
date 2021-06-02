////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/maya_animation/anim_track.h>
#include <xray/fs_utils.h>

using namespace xray::maya_animation;
using namespace xray::animation;
using namespace xray::math;

void anim_track::save( const char* fn )
{
	FILE*		f;
	bool const res	= fs::open_file( &f, fs::open_file_create | fs::open_file_truncate | fs::open_file_write, fn );
	if ( !res )
		return;

	save( f );

	fclose( f );
}

void anim_track::save( FILE* f )
{
	
	int ver				= 1;
	fwrite				( &ver, sizeof(ver), 1, f );
	
	for( int ch_id=0; ch_id<channel_max; ++ch_id )
	{
		EtCurve* c		= m_channels[ch_id];
		if(!c)			
		{
			int ch = -1;
			fwrite			( &ch, sizeof(ch), 1, f );

			continue;
		}
		fwrite			( &ch_id, sizeof(ch_id), 1, f );

		fwrite			( &c->numKeys,		sizeof(c->numKeys),		1, f );
		fwrite			( &c->isWeighted,	sizeof(c->isWeighted),	1, f );
		fwrite			( &c->isStatic,		sizeof(c->isStatic),	1, f );

//		int sz			= sizeof(c->preInfinity);
		fwrite			( &c->preInfinity,	sizeof(c->preInfinity),	1, f );
		fwrite			( &c->postInfinity,	sizeof(c->postInfinity), 1, f );
	
		fwrite			( c->keyListPtr,		sizeof(EtKey),			c->numKeys, f );
	}
}

void anim_track::load( const char* fn )
{
	FILE*				f;
	bool const res	= fs::open_file( &f, fs::open_file_read, fn );
	if ( !res )
		return;

	int count			= 0;
	fread				( &count, sizeof(count), 1, f );
	// read first track
	if (count!=0)
	{
		load			(f);
	}

	fclose(f);
}

void anim_track::load( FILE* f )
{

	int ver				= 0;
	fread				( &ver, sizeof(ver), 1, f );
	if(ver!=1)
		return;
	
	for( int ch_id=0; ch_id<channel_max; ++ch_id )
	{
		int				cid;
		fread			( &cid, sizeof(cid), 1, f );
		if(cid==-1)
			continue;
		m_channels[cid]	= NEW (EtCurve);
		EtCurve* c		= m_channels[cid];
		
		//initialize cache
		c->lastKey		= 0;
		c->lastIndex		= -1;
		c->lastInterval	= -1;
		c->isStep		= false;
		c->isStepNext	= false;


		fread			( &c->numKeys,		sizeof(c->numKeys),		1, f );
		fread			( &c->isWeighted,	sizeof(c->isWeighted),	1, f );
		fread			( &c->isStatic,		sizeof(c->isStatic),	1, f );

//		int sz = sizeof(c->preInfinity);
		fread			( &c->preInfinity,	sizeof(c->preInfinity),	1, f );
		fread			( &c->postInfinity,	sizeof(c->postInfinity), 1, f );
	
		c->create_keys	( c->numKeys );
		fread			(c->keyListPtr,		sizeof(EtKey),			c->numKeys, f );
	}

	reset_profile();
}

