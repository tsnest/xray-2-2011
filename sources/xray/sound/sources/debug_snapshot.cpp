////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_snapshot.h"


namespace xray {
namespace sound {

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

debug_snapshot::debug_snapshot					( XAPO_REGISTRATION_PROPERTIES const& reg_props ) :
	CXAPOBase				( &reg_props ),
	m_channels				( (u8)-1 ),
	m_bytes_per_sample		( (u8)-1 ),
	m_buffer				( 0 ),
	m_buffer_size_in_bytes	( 0 ),
	m_buffer_offset			( 0 )
{

}

HRESULT debug_snapshot::LockForProcess			(
									UINT32 InputLockedParameterCount,
									const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS *pInputLockedParameters,
									UINT32 OutputLockedParameterCount,
									const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS *pOutputLockedParameters )
{
    R_ASSERT				( !IsLocked() );
    R_ASSERT				( InputLockedParameterCount == 1 );
    R_ASSERT				( OutputLockedParameterCount == 1 );
    R_ASSERT				( pInputLockedParameters != NULL );
    R_ASSERT				( pOutputLockedParameters != NULL );
    R_ASSERT				( pInputLockedParameters[0].pFormat != NULL );
    R_ASSERT				( pOutputLockedParameters[0].pFormat != NULL );


	m_channels				= (u8)pInputLockedParameters[0].pFormat->nChannels;
	m_bytes_per_sample		= (u8)( pInputLockedParameters[0].pFormat->wBitsPerSample >> 3 );

	R_ASSERT				( m_buffer_size_in_bytes == 0 );
	R_ASSERT				( m_buffer == 0 );
	m_buffer_size_in_bytes	= pInputLockedParameters[0].pFormat->nAvgBytesPerSec * 3; // 3 sec
	m_buffer				= XRAY_MALLOC_IMPL ( debug::g_mt_allocator, m_buffer_size_in_bytes, "debug_snapshot_buffer" );
	memory::zero			( m_buffer, m_buffer_size_in_bytes );

    return CXAPOBase::LockForProcess(
        InputLockedParameterCount,
        pInputLockedParameters,
        OutputLockedParameterCount,
        pOutputLockedParameters);

}

void debug_snapshot::Process						(
									UINT32 InputProcessParameterCount,
									const XAPO_PROCESS_BUFFER_PARAMETERS *pInputProcessParameters,
									UINT32 OutputProcessParameterCount,
									XAPO_PROCESS_BUFFER_PARAMETERS *pOutputProcessParameters,
									BOOL IsEnabled )
{
	XRAY_UNREFERENCED_PARAMETER				( IsEnabled );
    R_ASSERT(InputProcessParameterCount == 1);
    R_ASSERT(OutputProcessParameterCount == 1);
    R_ASSERT(NULL != pInputProcessParameters);
    R_ASSERT(NULL != pOutputProcessParameters);


    XAPO_BUFFER_FLAGS inFlags = pInputProcessParameters[0].BufferFlags;
    XAPO_BUFFER_FLAGS outFlags = pOutputProcessParameters[0].BufferFlags;

    // R_ASSERT buffer flags are legitimate
    R_ASSERT(inFlags == XAPO_BUFFER_VALID || inFlags == XAPO_BUFFER_SILENT);
    R_ASSERT(outFlags == XAPO_BUFFER_VALID || outFlags == XAPO_BUFFER_SILENT);

    // check input APO_BUFFER_FLAGS
    switch (inFlags)
    {
    case XAPO_BUFFER_VALID:
        {
            pvoid pvSrc			= pInputProcessParameters[0].pBuffer;
            R_ASSERT			( pvSrc != NULL );

            pvoid pvDst			= pOutputProcessParameters[0].pBuffer;
            R_ASSERT			( pvDst != NULL );

			u32 size_in_bytes	= pInputProcessParameters[0].ValidFrameCount * m_channels * m_bytes_per_sample;
			memory::copy		( pvDst, size_in_bytes, pvSrc, size_in_bytes );

			R_ASSERT			( m_buffer_size_in_bytes % size_in_bytes == 0 );


			m_mutex.lock		( );
			memory::copy		( (u8*)m_buffer + m_buffer_offset, size_in_bytes, pvSrc, size_in_bytes );

			m_buffer_offset		+= size_in_bytes;
			if ( m_buffer_offset == m_buffer_size_in_bytes )
				m_buffer_offset	= 0;

			m_mutex.unlock		( );

			break;
        }

    case XAPO_BUFFER_SILENT:
        {
            // All that needs to be done for this case is setting the
            // output buffer flag to XAPO_BUFFER_SILENT which is done below.
            break;
        }

    }

    // set destination valid frame count, and buffer flags
    pOutputProcessParameters[0].ValidFrameCount = pInputProcessParameters[0].ValidFrameCount; // set destination frame count same as source
    pOutputProcessParameters[0].BufferFlags     = pInputProcessParameters[0].BufferFlags;     // set destination buffer flags same as source

}

void debug_snapshot::dump						( memory::writer& writer )
{
	threading::mutex_raii raii			( m_mutex );
	pvoid start							= (u8*)m_buffer + m_buffer_offset;
	writer.write						( start, m_buffer_size_in_bytes - m_buffer_offset );
	writer.write						( m_buffer, m_buffer_offset );
}

#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

} // namespace sound
} // namespace xray
