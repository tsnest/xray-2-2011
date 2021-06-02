////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

float g_mouse_sensitivity			= 1.0f;
bool g_allow_sensitivity_process	= false;

HHOOK mouse_events_hook				= NULL;

int round(float a)			{   return int(a + 0.5f);} 

LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
   // Let windows handle mouse clicks
   if (wParam != WM_MOUSEMOVE) 
      return CallNextHookEx(NULL, nCode, wParam, lParam);

   // Get event information
   PMSLLHOOKSTRUCT p	= reinterpret_cast<PMSLLHOOKSTRUCT>( lParam );
 
   // Old mouse location
   static float ox		= static_cast<float>( p->pt.x );
   static float oy		= static_cast<float>( p->pt.y );
 
   // Mouse location without sensitivity changed
   float x 				= static_cast<float>( p->pt.x );
   float y 				= static_cast<float>( p->pt.y );
 
   if(!g_allow_sensitivity_process)
   {
	   ox = x;
	   oy = y;
      return CallNextHookEx(NULL, nCode, wParam, lParam);
   }
   // New mouse location
   float nx 			= ox + (x - ox) * g_mouse_sensitivity;
   float ny 			= oy + (y - oy) * g_mouse_sensitivity;
 
   // Set cursor position
   SetCursorPos			( round( nx ), round( ny ) );

   // Set old location ready for next time
   ox = nx;
   oy = ny;

   return 1;
}


void install_hooks()
{
	HINSTANCE hInstance		= (HINSTANCE)System::Runtime::InteropServices::Marshal::GetHINSTANCE	(
								System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0] ).ToInt64();

   mouse_events_hook = SetWindowsHookEx(
                  WH_MOUSE_LL,      /* Type of hook */
                  mouseHookProc,    /* Hook process */
                  hInstance,        /* Instance */
                  NULL);

}

void remove_hooks()
{
	UnhookWindowsHookEx(mouse_events_hook);
}

