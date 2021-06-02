////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_device.h"

#include "hw_wrapper.h"

namespace xray {
namespace render {

void render_device::begin_scene_platform()
{
	R_CHK(hw_wrapper::get_ref().device()->BeginScene());
}

void render_device::end_scene_platform()
{
	ID3DDevice	*pdevice = hw_wrapper::get_ref().device();
	
	R_CHK(pdevice->EndScene());

	pdevice->Present( NULL, NULL, NULL, NULL );
	++m_frame;
}

void render_device::setup_states_platform() const
{
	hw_wrapper	&hw = hw_wrapper::get_ref();
	ID3DDevice	*pdevice = hw.device();

	const hw_caps& caps = hw.get_caps();

	for (u32 i=0; i<caps.raster.stages; i++)				
	{
		float fBias = -.5f	;
		R_CHK(pdevice->SetSamplerState	( i, D3DSAMP_MAXANISOTROPY, 4				));
		R_CHK(pdevice->SetSamplerState	( i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&fBias))));
		R_CHK(pdevice->SetSamplerState	( i, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR 		));
		R_CHK(pdevice->SetSamplerState	( i, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR 		));
		R_CHK(pdevice->SetSamplerState	( i, D3DSAMP_MIPFILTER,	D3DTEXF_LINEAR		));
	}

	R_CHK(pdevice->SetRenderState( D3DRS_DITHERENABLE,		TRUE				));
	R_CHK(pdevice->SetRenderState( D3DRS_COLORVERTEX,		TRUE				));
	R_CHK(pdevice->SetRenderState( D3DRS_ZENABLE,			TRUE				));
	R_CHK(pdevice->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_GOURAUD	));
	R_CHK(pdevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW			));
	R_CHK(pdevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATER		));
	R_CHK(pdevice->SetRenderState( D3DRS_LOCALVIEWER,		TRUE				));

	R_CHK(pdevice->SetRenderState( D3DRS_STENCILENABLE,		FALSE				));
	R_CHK(pdevice->SetRenderState( D3DRS_STENCILWRITEMASK,	0xFF				));

	R_CHK(pdevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL	));
	R_CHK(pdevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL	));
	R_CHK(pdevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL	));
	R_CHK(pdevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	));
	R_CHK(pdevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,	FALSE			));
	R_CHK(pdevice->SetRenderState( D3DRS_NORMALIZENORMALS,		TRUE			));

//	if (psDeviceFlags.test(rsWireframe))	{ R_CHK(pdevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_WIREFRAME	)); }
//	else									{ R_CHK(pdevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID		)); }
	R_CHK(pdevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID		));

	// ******************** Fog parameters
	R_CHK(pdevice->SetRenderState( D3DRS_FOGCOLOR,			0					));
	R_CHK(pdevice->SetRenderState( D3DRS_RANGEFOGENABLE,	FALSE				));
//	if (caps.table_fog )	
//	{
		R_CHK(pdevice->SetRenderState( D3DRS_FOGTABLEMODE,	D3DFOG_LINEAR		));
		R_CHK(pdevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_NONE			));
//	} else {
		R_CHK(pdevice->SetRenderState( D3DRS_FOGTABLEMODE,	D3DFOG_NONE			));
		R_CHK(pdevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR		));
//	}

}

render_device::device_state render_device::get_device_state() const
{
	hw_wrapper	&hw = hw_wrapper::get_ref();
	ID3DDevice	*pdevice = hw.device();
	HRESULT	_hr		= pdevice->TestCooperativeLevel();
	if (FAILED(_hr))
	{
		// If the device was lost, do not render until we get it back
		if		(D3DERR_DEVICELOST==_hr)
			return ds_lost;

		// Check if the device is ready to be reset
		if		(D3DERR_DEVICENOTRESET==_hr)
			return ds_need_reset;
	}

	return ds_ok;
}
} // namespace render 
} // namespace xray 
