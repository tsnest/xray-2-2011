////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef VALUE_ATTRIBUTE_H_INCLUDED
#define VALUE_ATTRIBUTE_H_INCLUDED

using System::AttributeTargets;


[System::AttributeUsage(AttributeTargets::Property, AllowMultiple=true)]
public ref class ValueAttribute : public System::Attribute 
{
public:
	enum class value_type{e_min_val, e_max_val, e_def_val};
	ValueAttribute(value_type v0, float v){
		create			(v0); 
		m_params->Add	(v);
	}
	ValueAttribute(value_type v0, float v1, float v2){
	   create			(v0); 
	   m_params->Add	(v1);
	   m_params->Add	(v2);
	}
	ValueAttribute(value_type v0, float v1, float v2, float v3){
	   create			(v0); 
	   m_params->Add	(v1);
	   m_params->Add	(v2);
	   m_params->Add	(v3);
	}
	ValueAttribute(value_type v0, float v1, float v2, float v3, float v4){
	   create			(v0); 
	   m_params->Add	(v1);
	   m_params->Add	(v2);
	   m_params->Add	(v3);
	   m_params->Add	(v4);
	}
	ValueAttribute(value_type v0, int v){
		create			(v0); 
		m_params->Add	(v);
	}
	ValueAttribute(value_type v0, bool v){
		create			(v0); 
		m_params->Add	(v);
	}
	ValueAttribute(value_type v0, System::String^ v){
		create			(v0); 
		m_params->Add	(v);
	}

	System::Collections::ArrayList^ m_params;
	value_type						m_type; 
private:
	void create(value_type v0){m_params=gcnew System::Collections::ArrayList; m_type = v0;}
}; //ValueAttribute

#endif // #ifndef VALUE_ATTRIBUTE_H_INCLUDED
