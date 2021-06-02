////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COM_UTILS_H_INCLUDED
#define COM_UTILS_H_INCLUDED

template <class T>
void safe_release(T*& object)
{
	if (object)
	{
		object->Release();
		object = 0;
	}
}

template <class T>
void log_ref_count(pcstr object_name, T* object)
{
	ASSERT(object);
	
	object->AddRef(); 
	ULONG ref_count = object->Release();

	//LOG_DEBUG("refCount:%s: %d", object_name, ref_count);
	LOG_INFO("%-20s:refCount: %d", object_name, ref_count);
}


#endif // #ifndef COM_UTILS_H_INCLUDED