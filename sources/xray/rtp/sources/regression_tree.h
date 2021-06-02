////////////////////////////////////////////////////////////////////////////
//	Created		: 05.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef REGRESSION_TREE_H_INCLUDED
#define REGRESSION_TREE_H_INCLUDED

#include "iaction.h"

namespace xray {

namespace configs{
	class lua_config_value;
} // namespace configs

namespace render {
namespace debug{

	class renderer;

} // namespace debug{
} // namespace render 

namespace rtp {

template< class tree_space_param >
class training_save_load_data;

template< class tree_space_param >
class node_save_load_data
{
public:
	virtual training_save_load_data<tree_space_param>* get_training_save_load_data() { return 0; }

public:
	virtual const training_save_load_data< tree_space_param >* get_training_save_load_data()const { return 0; }
	virtual	~node_save_load_data	() { }
}; // class node_save_load_data

template< class space_param >
class node
{

public:
typedef	space_param				space_param_type ;
typedef node< space_param >		self_type;

public:
			node		();
			node		( u32 level );

	virtual	~node		();

	float	reward		(  const space_param_type &params  ) const ;

public:
			bool			is_leaf		( ) const;

public:
			bool			compare		( const self_type &node ) const;

			const node*		left		( ) const;
			const node*		right		( ) const;

public://protected?
	virtual	bool	build		( ){ ASSERT(false); return false; }
	virtual	bool	recalculate ( ){ ASSERT(false); return false; }



public:
#ifndef MASTER_GOLD
	virtual	void	save		( xray::configs::lua_config_value cfg, const node_save_load_data< space_param_type >& data )const;
#endif //MASTER_GOLD

	virtual	void	load		( const xray::configs::binary_config_value &cfg, node_save_load_data< space_param_type >& data  );

private:
	virtual	node	*create		( u32 level );
	virtual	u32		level		( ) { return u32(-1); };

protected:
	node*			m_left;
	node*			m_right;

	u32				m_split_dimension;
	float			m_split_value;

	float			m_reward_value;

}; // class node


template< class node >
class regression_tree 
{
public:
	typedef				node node_type;

protected:
	typedef typename	node_type::space_param_type	space_param_type; 

//private:
//typedef	iaction< space_param_type >	iaction_type;

public:
			regression_tree( ):m_root( 0 ){};

protected:
			regression_tree	( u32 min_samples ): m_root( 0, min_samples ){};
		//regression_tree	(  const typename node::training_sample_type **begin,  const typename node::training_sample_type **end, const space_param_type &min, const space_param_type &max, u32 min_samples ): m_root( 0,  begin, end, min, max, min_samples ){};

public:
	virtual	~regression_tree(){}

public:
	float	reward			(  const space_param_type &params  ) const 
	{
		return m_root.reward( params );
	}
	
	bool	compare			( const regression_tree< node > &tree ) const
	{
		return m_root.compare( tree.m_root );
	}

public:
#ifndef MASTER_GOLD
virtual	void	save		( xray::configs::lua_config_value cfg, const node_save_load_data< space_param_type >& data  )const;
#endif // MASTER_GOLD

virtual	void	load		( const xray::configs::binary_config_value &cfg, node_save_load_data< space_param_type >& data );

//virtual	void	render		( xray::render::debug::renderer&, space_param_type const &to ) const{};

protected:
	node_type			m_root;

}; // class regression_tree



} // namespace rtp
} // namespace xray

#include "regression_tree_inline.h"

#endif // #ifndef REGRESSION_TREE_H_INCLUDED