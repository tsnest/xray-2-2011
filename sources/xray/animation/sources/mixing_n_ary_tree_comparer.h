////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_COMPARER_H_INCLUDED
#define MIXING_N_ARY_TREE_COMPARER_H_INCLUDED

namespace xray {
namespace animation {

struct base_interpolator;
class bone_mixer;
	
namespace mixing {

class n_ary_tree;
struct binary_tree_base_node;

struct n_ary_tree_base_node;
class n_ary_tree_animation_node;

struct animation_state;

class n_ary_tree_comparer {
public:
					n_ary_tree_comparer			(
						n_ary_tree const& from,
						n_ary_tree const& to,
						u32 current_time_in_ms
					);
			bool	equal						( ) const;
			u32		needed_buffer_size			( ) const;
	inline	u32		animations_count			( ) const { return m_animations_count; }

private:
					n_ary_tree_comparer			( n_ary_tree_comparer const& );
	n_ary_tree_comparer& operator =				( n_ary_tree_comparer const& );
			void	process_animations			(
						n_ary_tree_animation_node*& i,
						n_ary_tree_animation_node*& j
					);

private:
	template < typename T >
	inline	void	increase_buffer_size		( T& node );
			void	increase_buffer_size		( n_ary_tree_base_node& node );

			void	new_animation				( n_ary_tree_animation_node& animation );
			void	new_weight_transition		( float from, float to );
			void	new_time_scale_transition	(
						n_ary_tree_base_node& from,
						n_ary_tree_base_node& to
					);
			void	new_time_scale_transition	(
						n_ary_tree_base_node& from,
						float to
					);
			void	new_time_scale_transition	(
						float from,
						n_ary_tree_base_node& to
					);
			void	new_weight_transition			(
						n_ary_tree_base_node& from,
						n_ary_tree_base_node& to
					);
			void	new_weight_transition			(
						base_interpolator const& from_animation_interpolator,
						n_ary_tree_base_node& from,
						float to
					);
			void	new_weight_transition			(
						base_interpolator const& to_animation_interpolator,
						float from,
						n_ary_tree_base_node& to
					);

			void	add_operands											(
						n_ary_tree_animation_node& from,
						n_ary_tree_animation_node& to,
						bool const skip_time_scale_node
					);

			void	add_animation				(
						n_ary_tree_animation_node& animation,
						base_interpolator const& interpolator
					);
			void	remove_animation			(
						n_ary_tree_animation_node& animation,
						n_ary_tree_animation_node const* const driving_animation,
						base_interpolator const& interpolator
					);
			void	change_animation										(
						n_ary_tree_animation_node& from,
						n_ary_tree_animation_node& to
					);

			void	merge_asynchronous_groups	(
						n_ary_tree_animation_node* const from_begin,
						n_ary_tree_animation_node* const from_end,
						n_ary_tree_animation_node* const to_begin,
						n_ary_tree_animation_node* const to_end
					);
			void	merge_synchronization_groups							(
						n_ary_tree_animation_node* from_begin,
						n_ary_tree_animation_node* from_end,
						n_ary_tree_animation_node* to_begin,
						n_ary_tree_animation_node* to_end,
						n_ary_tree_animation_node& new_driving_animation,
						base_interpolator const& interpolator
					);

			void	new_driving_animation		(
						n_ary_tree_animation_node& animation,
						base_interpolator const& interpolator
					);
			void	new_driving_animation		(
						n_ary_tree_animation_node& new_driving_animation,
						n_ary_tree_animation_node& new_driving_animation_in_previous_target,
						bool skip_time_scale_node
					);

			void	add_synchronization_group	(
						n_ary_tree_animation_node* begin,
						n_ary_tree_animation_node* end
					);
			void	remove_synchronization_group(
						n_ary_tree_animation_node* begin,
						n_ary_tree_animation_node* end
					);
			void	change_synchronization_group(
						n_ary_tree_animation_node* from_begin,
						n_ary_tree_animation_node* from_end,
						n_ary_tree_animation_node* to_begin,
						n_ary_tree_animation_node* to_end,
						u32 current_time_in_ms
					);

			void	merge_trees					(
						n_ary_tree const& from,
						n_ary_tree const& to,
						u32 current_time_in_ms
					);

			void	process_interpolators		(
						n_ary_tree const& from,
						n_ary_tree const& to
					);

private:
	u32							m_animations_count;
	u32							m_needed_buffer_size;
	bool						m_equal;
}; // class n_ary_tree_comparer

class animation_comparer_less_predicate {
public:
	inline	animation_comparer_less_predicate	( bool const use_synchronized_animations = true ) :
		m_use_synchronized_animations( use_synchronized_animations )
	{
	}
	
			bool	operator ( )				( n_ary_tree_animation_node const& left, n_ary_tree_animation_node const& right ) const;
	inline	bool	operator ( )				( n_ary_tree_animation_node const* const left, n_ary_tree_animation_node const* const right ) const
	{
		return		operator ( ) ( *left, *right );
	}

private:
	bool	m_use_synchronized_animations;
}; // class animation_comparer_less_predicate

struct animation_comparer_equal_predicate {
	bool operator ( )							( n_ary_tree_animation_node const& left, n_ary_tree_animation_node const& right ) const;
};

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_COMPARER_H_INCLUDED