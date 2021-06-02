////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DOMAIN_TYPE_HOLDER_H_INCLUDED
#define PARTICLE_DOMAIN_TYPE_HOLDER_H_INCLUDED

namespace xray {
	namespace particle_editor{

public ref class particle_domain_type_holder : xray::editor::wpf_controls::i_property_value {
public:
	particle_domain_type_holder();
	

private:
	System::String^			m_selected_value;


public: 
	virtual void			set_value		( System::Object^ value );
	virtual System::Object^	get_value		( ){ return m_selected_value; }

	virtual property System::Type^			value_type
	{
		System::Type^ get( ) { return System::String::typeid; }
	}
}; // class particle_domain_type_holder

} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_DOMAIN_TYPE_HOLDER_H_INCLUDED