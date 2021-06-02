
inline void rename_cfg(xray::configs::lua_config_value* v, System::String^ new_field_id )
{
	xray::editor::unmanaged_string		s(new_field_id);
	v->rename							(s.c_str());
}

inline void set_c_string(xray::configs::lua_config_value v, System::String^ str )
{
	xray::editor::unmanaged_string		s(str);
	v									= (s.c_str());
}
