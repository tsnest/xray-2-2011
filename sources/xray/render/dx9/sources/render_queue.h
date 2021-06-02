////////////////////////////////////////////////////////////////////////////
//	Created		: 06.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_QUEUE_H_INCLUDED
#define RENDER_QUEUE_H_INCLUDED

#include <xray/render/common/sources/res_shader.h>

namespace xray {
namespace render{

// Forward declarations;
class render_visual;

union key_rq
{
#pragma warning(push)
#pragma warning(disable: 4201)
	struct
	{
		u64	id:14;
		u64	priority:50;
	};
#pragma warning(pop)

	u64	qword;
};

//template<typename base>
struct queue_item//: public base
{
	ref_pass		pass;
	render_visual*	visual;
	float			lod;
};

struct key_rq_sort_desc
{
	bool operator()(const key_rq& lhs, const key_rq& rhs)
	{
		return lhs.qword > rhs.qword;
	}
};

template<typename key_type, typename value_type, typename sort_pred>
class render_queue
{
public:
	void swap(render_queue<key_rq, queue_item, key_rq_sort_desc>& other)
	{
		m_keys.swap(other.m_keys);
		m_dips.swap(other.m_dips);
	}

	void add_dip(key_type key, const value_type& value)
	{
		key.id = m_keys.size();
		ASSERT(key.id<(1<<14));
		m_keys.push_back(key);
		m_dips.push_back(value);
	}
	
	void render(bool do_clear)
	{
		prepere();

		for (u32 i = 0; i < m_keys.size(); ++i)
		{
			value_type& item = m_dips[m_keys[i].id];
			item.pass->apply();
			item.visual->render();
		}

		if (do_clear)
		{
			clear();
		}
	}

	bool empty() {return m_keys.empty();}
	
	u32  size() {ASSERT(m_keys.size()==m_dips.size()); return m_keys.size();}

private:
	void prepere()
	{
		std::sort(m_keys.begin(), m_keys.end(), sort_pred());
	}

	void clear()
	{
		m_keys.clear();
		m_dips.clear();
	}

private:
	render::vector<key_type>	m_keys;
	render::vector<value_type>	m_dips;
}; // class render_queue

//struct static_base
//{
//	render_visual*	visual;
//};
//
//typedef	queue_item<static_base> static_item;
//
//union key_rq
//{
//	enum
//	{
//		vs_id_bit_width = 7,
//		gs_id_bit_width = 4,
//		ps_id_bit_width = 7,
//		state_id_bit_width = 7,
//		tex_list_id_bit_width = 12,
//		id_bit_width = 14
//	};
//
//	//it is written with assumption that default order of fields in union is lsb to msb
//	struct  
//	{
//		u64	m_id			: id_bit_width;
//		u64 m_texlist_id	: texlistid_bit_width;
//		u64	m_state_id		: stateid_bit_width;	
//		u64	m_ps_id			: psid_bit_width;
//#ifdef use_dx10
//		u64	m_gs_id			: gsid_bit_width;
//#endif
//		u64	m_vs_id			: vsid_bit_width;
//	};
//	u64		m_qword;
//
//#ifdef USE_DX10
//	key_rq(res_vs* vs, res_gs* gs, res_ps* ps, res_state* state, res_texture_list* tlist, u32 id, float dist_sq)
//#else
//	key_rq(res_vs* vs, res_ps* ps, res_state* state, res_texture_list* tlist, u32 id, float dist_sq)
//#endif
//	{
//		m_qword = 0;
//		u64 vs_id = vs->get_id();
//		ASSERT(vs_id < (1<<vsid_bit_width));
//		m_vs_id = vs_id;
//#ifdef use_dx10
//		u64 gs_id = gs->get_id();
//		ASSERT(gs_id < (1<<gsid_bit_width));
//		m_gsid = gs_id;
//#endif
//		u64 ps_id = ps->get_id();
//		ASSERT(psid < (1<<ps_id_bit_width));
//		m_ps_id = ps_id;
//		u64 state_id = state->get_id();
//		ASSERT(state_id < (1<<stateid_bit_width));
//		m_state_id   = state_id;
//		u64 tlist_id = tlist->get_id();
//		ASSERT(tlist_id < (1<<texlistid_bit_width));
//		m_texlist_id = tlist_id;
//
//		ASSERT(id < (1<<id_bit_width));
//		m_id = id;
//	}
//};
//
//struct key_dist
//{
//	float	m_dist;
//	u32		m_id;
//#ifdef USE_DX10
//	KeyDist(res_vs* vs, res_gs* gs, res_ps* ps, res_state* state, res_texture_list* tlist, u32 id, float dist_sq)
//#else
//	KeyDist(res_vs* vs, res_ps* ps, res_state* state, res_texture_list* tlist, u32 id, float dist_sq)
//#endif
//	{
//		m_dist = dist_sq;
//		m_id = id;
//	}
//};
//
//struct  key_dist_sort_asc
//{
//	inline bool operator()(const key_dist& lhs, const key_dist& rhs)
//	{
//		return lhs.m_Dist < rhs.m_Dist;
//	}
//};
//
//struct  key_dist_sort_desc
//{
//	inline bool operator()(const key_dist& lhs, const key_dist& rhs)
//	{
//		return lhs.m_Dist > rhs.m_Dist;
//	}
//};
//
//struct key_rq_sort_desc
//{
//	bool operator()(const key_rq& lhs, const key_rq& rhs)
//	{
//		return lhs.m_qword > rhs.m_qword;
//	}
//};
//
//template<typename KeyType, typename ValueType, typename SortPred>
//class RenderQueue
//{
//public:
//	void AddDIP(ref_pass Pass, float ssa, float distSQ, dxRender_Visual* pVisual, const Fmatrix& transform, IRenderable* pObject);
//	void Prepere();
//	void Render(bool bClear);
//	void Clear();
//	bool Empty(){return m_Keys.empty();}
//
//private:
//};

}// namespace render
}// namespace xray


#endif // #ifndef RENDER_QUEUE_H_INCLUDED