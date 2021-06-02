////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CONVERTING_H_INCLUDED
#define CONVERTING_H_INCLUDED

inline System::String^ convert_to_string( float value )
{
	char buffer[256];
	_gcvt_s	( buffer, value, 8 );
	return	gcnew System::String( buffer );
}

inline float convert_to_single( System::String^ string )
{
	size_t					convertedChars = 0;
	size_t					sizeInBytes = ( ( string->Length + 1 ) * 2 );
	pstr const native_string	= ( pstr ) ALLOCA( (u32)sizeInBytes );
	pin_ptr<const wchar_t>	wch = PtrToStringChars(string);
	R_ASSERT				(
		!wcstombs_s	(
		&convertedChars, 
		native_string,
		sizeInBytes,
		wch,
		sizeInBytes
		),
		"[tostring][failed] : wcstombs_s failed"
		);

	return					(float)atof( native_string );
}

inline float convert_to_single( System::Object^ string )
{
	return	convert_to_single( safe_cast<System::String^>(string) );
}
#endif // #ifndef CONVERTING_H_INCLUDED