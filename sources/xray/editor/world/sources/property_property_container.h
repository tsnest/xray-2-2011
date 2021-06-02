////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_PROPERTY_CONTAINER_HPP_INCLUDED
#define PROPERTY_PROPERTY_CONTAINER_HPP_INCLUDED

#include "property_holder.h"


public ref class property_property_container : public xray::editor::controls::property_value {
public:
							property_property_container	(xray::editor::property_holder_ptr object);
	virtual System::Object	^get_value					();
	virtual void			set_value					(System::Object ^object);
private:
	!property_property_container() 
	{
		DELETE(m_object);
	}
	~property_property_container() 
	{
		this->!property_property_container();
	}
private:
	xray::editor::property_holder_ptr*		m_object;
}; // ref class property_property_container

#endif // ifndef PROPERTY_PROPERTY_CONTAINER_HPP_INCLUDED