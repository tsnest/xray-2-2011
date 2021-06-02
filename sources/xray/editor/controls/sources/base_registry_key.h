////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_REGISTRY_KEY_H_INCLUDED
#define BASE_REGISTRY_KEY_H_INCLUDED

using namespace Microsoft::Win32;

namespace xray {

public ref class base_registry_key {
public:
	static String^ company_name;
	static String^ product_name;
	static RegistryKey^ get	()
	{
		RegistryKey^		software = Registry::CurrentUser->OpenSubKey( "Software", true );
		ASSERT				(software);

		RegistryKey^		company = software->OpenSubKey( company_name, true );
		if (!company)
			company			= software->CreateSubKey( company_name );
		ASSERT				(company);
		software->Close		();

		RegistryKey^		product = company->OpenSubKey( product_name, true );
		if (!product)
			product			= company->CreateSubKey( product_name );

		ASSERT				(product);
		company->Close		();

		return				(product);
	}
	static RegistryKey^  get_sub_key(RegistryKey^ root, System::String^ name)
	{
		RegistryKey^ result = root->OpenSubKey(name, true);
		if(!result)
			result			= root->CreateSubKey(name);

		return result;
	}	
	
}; // class base_registry_key


}//namespace xray

#endif // #ifndef BASE_REGISTRY_KEY_H_INCLUDED