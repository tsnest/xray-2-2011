////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_COMMON_INLINE_H_INCLUDED
#define RES_COMMON_INLINE_H_INCLUDED

//namespace xray {
//namespace render_dx10 {


template<typename T>
void res_base::destroy(T* object)
{
//	res_base* this_mut = const_cast<res_base*>(this);
	object->T::_free();
	//resource_manager::ref().release( object);
	//DELETE(object);
}

//} // namespace render 
//} // namespace xray 


#endif // #ifndef RES_COMMON_INLINE_H_INCLUDED