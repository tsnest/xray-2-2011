////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_SNAPSHOT_H_INCLUDED
#define DEBUG_SNAPSHOT_H_INCLUDED

namespace xray {
namespace sound {

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

class debug_snapshot : public CXAPOBase
{
public:
								debug_snapshot				( XAPO_REGISTRATION_PROPERTIES const& reg_props );

			void				dump						( memory::writer& writer );
public:

	virtual HRESULT	_stdcall	LockForProcess				(
									UINT32 InputLockedParameterCount,
									const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS *pInputLockedParameters,
									UINT32 OutputLockedParameterCount,
									const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS *pOutputLockedParameters );

	virtual void	_stdcall	Process						(
									UINT32 InputProcessParameterCount,
									const XAPO_PROCESS_BUFFER_PARAMETERS *pInputProcessParameters,
									UINT32 OutputProcessParameterCount,
									XAPO_PROCESS_BUFFER_PARAMETERS *pOutputProcessParameters,
									BOOL IsEnabled );
private:
	threading::mutex	m_mutex;
	pvoid				m_buffer;
	u32					m_buffer_size_in_bytes;
	u32					m_buffer_offset;
	u8					m_channels;
	u8					m_bytes_per_sample;
};

#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
class debug_snapshot
{};
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

} // namespace sound
} // namespace xray

#endif // #ifndef DEBUG_SNAPSHOT_H_INCLUDED