////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_PARAMETERS_H_INCLUDED
#define XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_PARAMETERS_H_INCLUDED

#include <xray/animation/noncopyable.h>
#include <xray/animation/mixing.h>
#include <xray/animation/mixing_animation_interval.h>
#include <xray/animation/mixing_animation_lexeme.h>

namespace xray {
namespace animation {

struct base_interpolator;

namespace mixing {

class XRAY_ANIMATION_API animation_lexeme_parameters : private animation::noncopyable {
public:
	inline									animation_lexeme_parameters			(
												mutable_buffer& buffer,
												pcstr identifier,
												animation_interval const* animation_intervals_begin,
												animation_interval const* animation_intervals_end
											);
	template < int AnimationIntervalsCount >
	inline									animation_lexeme_parameters			(
												mutable_buffer& buffer,
												pcstr identifier,
												animation_interval const (&animation_intervals)[ AnimationIntervalsCount ]
											);
	inline									animation_lexeme_parameters			(
												mutable_buffer& buffer,
												pcstr identifier,
												animation_interval const& animation_interval
											);
	inline									animation_lexeme_parameters			(
												mutable_buffer& buffer,
												pcstr identifier,
												skeleton_animation_ptr const& animation
											);
	inline									animation_lexeme_parameters			(
												mutable_buffer& buffer,
												pcstr identifier,
												animation_interval const* animation_intervals_begin,
												animation_interval const* animation_intervals_end,
												animation_lexeme& driving_animation
											);
	template < int AnimationIntervalsCount >
	inline									animation_lexeme_parameters			(
												mutable_buffer& buffer,
												pcstr identifier,
												animation_interval const (&animation_intervals)[ AnimationIntervalsCount ],
												animation_lexeme& driving_animation
											);
	inline									animation_lexeme_parameters			(
												mutable_buffer& buffer,
												pcstr identifier,
												animation_interval const& animation_interval,
												animation_lexeme& driving_animation
											);
	inline									animation_lexeme_parameters			(
												mutable_buffer& buffer,
												pcstr identifier,
												skeleton_animation_ptr const& animation,
												animation_lexeme& driving_animation
											);
	// setters
	inline	animation_lexeme_parameters&	weight_interpolator					( base_interpolator const& interpolator			) { m_weight_interpolator				= &interpolator						; return *this; }
	inline	animation_lexeme_parameters&	time_scale_interpolator				( base_interpolator const& interpolator			) { m_time_scale_interpolator			= &interpolator						; return *this; }
#ifndef MASTER_GOLD
	inline	animation_lexeme_parameters&	user_data							( u32 const user_data							) { m_user_data							= user_data							; return *this; }
#endif // #ifndef MASTER_GOLD
	inline	animation_lexeme_parameters&	start_animation_interval_id			( u32 const start_animation_interval_id			);
	inline	animation_lexeme_parameters&	start_animation_interval_time		( float const start_animation_interval_time		);
	inline	animation_lexeme_parameters&	start_cycle_animation_interval_id	( u32 const start_cycle_animation_interval_id	);
	inline	animation_lexeme_parameters&	time_scale							( float const time_scale						) { m_time_scale						= time_scale						; return *this; }
	inline	animation_lexeme_parameters&	playing_type						( playing_type_enum const playing_type			) { m_playing_type						= playing_type						; return *this; }
	inline	animation_lexeme_parameters&	synchronization_group_id			( u32 const synchronization_group_id			) { m_synchronization_group_id			= synchronization_group_id			; return *this; }
	inline	animation_lexeme_parameters&	additivity_priority					( u32 const additivity_priority					) { m_additivity_priority				= additivity_priority				; return *this; }
	inline	animation_lexeme_parameters&	override_existing_animation			( bool const override_existing_animation		) { m_override_existing_animation		= override_existing_animation		; return *this; }

	// acceptors
	inline	mutable_buffer&					buffer								( ) const { return m_buffer								; }
	inline	animation_interval const*		animation_intervals					( ) const { return m_animation_intervals				; }
	inline	base_interpolator const*		weight_interpolator					( ) const { return m_weight_interpolator				; }
	inline	base_interpolator const*		time_scale_interpolator				( ) const { return m_time_scale_interpolator			; }
	inline	animation_lexeme* const			driving_animation					( ) const { return m_driving_animation					; }
#ifndef MASTER_GOLD
	inline	pcstr							identifier							( ) const { return m_identifier							; }
	inline	u32								user_data							( ) const { return m_user_data							; }
#endif // #ifndef MASTER_GOLD
	inline	u32 const						animation_intervals_count			( ) const { return m_animation_intervals_count			; }
	inline	u32								start_animation_interval_id			( ) const { return m_start_animation_interval_id		; }
	inline	float							start_animation_interval_time		( ) const { return m_start_animation_interval_time		; }
	inline	u32								start_cycle_animation_interval_id	( ) const { return m_start_cycle_animation_interval_id	; }
	inline	float							time_scale							( ) const { return m_time_scale							; }
	inline	playing_type_enum				playing_type						( ) const { return m_playing_type						; }
	inline	u32								synchronization_group_id			( ) const { return m_synchronization_group_id			; }
	inline	u32								additivity_priority					( ) const { return m_additivity_priority				; }
	inline	bool							override_existing_animation			( ) const { return m_override_existing_animation		; }

private:
			void							create_animation_intervals			( skeleton_animation_ptr const& animation );
	static	u32								animation_intervals_count			( skeleton_animation_ptr const& animation );

private:
	mutable_buffer&							m_buffer;
	// it's a pity, but the order of the next 3 members is important
#ifndef MASTER_GOLD
	pcstr const								m_identifier;
#endif // #ifndef MASTER_GOLD
	animation_lexeme* const					m_driving_animation;
	animation_interval const* const			m_animation_intervals;
	base_interpolator const*				m_weight_interpolator;
	base_interpolator const*				m_time_scale_interpolator;
#ifndef MASTER_GOLD
	u32										m_user_data;
#endif // #ifndef MASTER_GOLD
	u32 const								m_animation_intervals_count;
	u32										m_start_cycle_animation_interval_id;
	u32										m_start_animation_interval_id;
	float									m_start_animation_interval_time;
	float									m_time_scale;
	playing_type_enum						m_playing_type;
	u32										m_synchronization_group_id;
	u32										m_additivity_priority;
	bool									m_override_existing_animation;
}; // class mixing_animation_lexeme_parameters

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_animation_lexeme_parameters_inline.h>

#endif // #ifndef XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_PARAMETERS_H_INCLUDED