////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_COMMON_INLINE_H_INCLUDED
#define RES_COMMON_INLINE_H_INCLUDED

//namespace xray {
//namespace render {


template<typename T>
void res_base::destroy(T* object)
{
	DELETE(object);
}

//} // namespace render 
//} // namespace xray 


#endif // #ifndef RES_COMMON_INLINE_H_INCLUDED