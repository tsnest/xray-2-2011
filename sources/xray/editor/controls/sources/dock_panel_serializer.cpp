////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "dock_panel_serializer.h"
#include "base_registry_key.h"

using namespace Microsoft::Win32;
using namespace WeifenLuo::WinFormsUI::Docking;

namespace xray {
namespace editor {
namespace controls{

	template <typename T>
	inline static T registry_value			(RegistryKey ^key, String ^value_id, const T &default_value)
	{
		array<String^>^		names = key->GetValueNames();
		if (names->IndexOf(names,value_id) >= 0)
			return			((T)key->GetValue(value_id));

		return				(default_value);
	}

	void serializer::serialize_dock_panel_root			(Form^ root, DockPanel^ panel, String^ window_name)
	{
		using System::IO::MemoryStream;
		

		RegistryKey^			product = base_registry_key::get();
		ASSERT					(product);

		RegistryKey^			windows = product->CreateSubKey("windows");
		
		RegistryKey^			dock_panel = windows->CreateSubKey(window_name);
		MemoryStream^			stream = gcnew MemoryStream();
		panel->SaveAsXml		(stream, System::Text::Encoding::Unicode, true);
		stream->Seek			(0, System::IO::SeekOrigin::Begin);
		dock_panel->SetValue	("panels",stream->ToArray());
		delete					stream;
		
		dock_panel->SetValue	("window_state", 2);
		switch (root->WindowState) {
			case FormWindowState::Maximized :
				dock_panel->SetValue	("window_state", 1);
			case FormWindowState::Minimized :
			{
				RegistryKey^		position = dock_panel->CreateSubKey("position");
				position->SetValue	("left",	root->RestoreBounds.X);
				position->SetValue	("top",		root->RestoreBounds.Y);
				position->SetValue	("width",	root->RestoreBounds.Width);
				position->SetValue	("height",	root->RestoreBounds.Height);
				position->Close		();
	
			break;
			}
			default : {
				dock_panel->SetValue	("window_state", 2);
				{
					RegistryKey^		position = dock_panel->CreateSubKey("position");
					position->SetValue	("left",	root->Location.X);
					position->SetValue	("top",		root->Location.Y);
					position->SetValue	("width",	root->Size.Width);
					position->SetValue	("height",	root->Size.Height);
					position->Close		();
				}
				break;
			}
		}

		dock_panel->Close				();

		windows->Close			();
		product->Close			();
	}

void clear_contents( DockPanel^ panel )
{
	System::Collections::ArrayList^ contents    = gcnew System::Collections::ArrayList(panel->Contents);
     for each (WeifenLuo::WinFormsUI::Docking::DockContent^ i in contents) 
         i->DockPanel    = nullptr;

     delete                contents;
}
	bool serializer::deserialize_dock_panel_root	(Form^ root, DockPanel^ panel, String^ window_name, DeserializeDockContent^ get_panel_for_setting_callback, bool reload )
	{
		if(reload && panel->Contents->Count)
			clear_contents( panel );

		Boolean is_load_success	= false;
		root->SuspendLayout			( );

 		root->Width				= 800;
		root->Height			= 600;

		RegistryKey			^product = base_registry_key::get();
		ASSERT				(product);

		RegistryKey			^windows = product->OpenSubKey("windows");
		if (windows) {
			RegistryKey			^dock_panel = windows->OpenSubKey(window_name);
			if (dock_panel) {
				Object^				temp = dock_panel->GetValue("panels");
				
				if (temp) {
					System::Array^	object = safe_cast<System::Array^>(dock_panel->GetValue("panels"));
					
					windows->Close	();
					delete			(windows);

					product->Close	();
					delete			(product);

					System::IO::MemoryStream^ stream = gcnew System::IO::MemoryStream();
					stream->Write		(safe_cast<array<unsigned char,1>^>(object),0,object->Length);
					stream->Seek		(0,System::IO::SeekOrigin::Begin);
					panel->LoadFromXml	(stream, get_panel_for_setting_callback);
					root->ResumeLayout	();
					is_load_success		= true;
				}

				RegistryKey		^position = dock_panel->OpenSubKey("position");
				if (position) {
					root->Left		= (int)registry_value(position,"left",root->Left);
					root->Top		= (int)registry_value(position,"top",root->Top);
					root->Width		= (int)registry_value(position,"width",root->Width);
					root->Height	= (int)registry_value(position,"height",root->Height);

					root->Location	= Point(root->Left, root->Top);

					position->Close	();
				}
				switch ((int)registry_value(dock_panel,"window_state",2)) {
					case 1 : {
						root->WindowState	= FormWindowState::Maximized;
						break;
					}
					case 2 : {
						root->WindowState	= FormWindowState::Normal;
						break;
					}
					default : NODEFAULT();
				}

				dock_panel->Close	();
			}
			windows->Close		();
			delete				(windows);
		}

		product->Close		();
		delete				(product);

		root->ResumeLayout		();

		return					is_load_success;//!!windows;
	}

}//namespace controls
}//namespace edotor
}//namespace xray