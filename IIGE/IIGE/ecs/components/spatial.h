#pragma once

#include <utils/math/geometry/shapes.h>
#include <utils/math/math.h>
#include <utils/template_wrappers.h>

#include "collision.h"
#include "component.h"

namespace iige::ecs::components::transform
	{
	struct relative
		{
		struct translation : component<utils::math::vec2f, "relative_translation"    > 
			{ 
			using component<utils::math::vec2f, "relative_translation">::component;
			using min = component<utils::math::vec2f, "relative_translation_min">; 
			using max = component<utils::math::vec2f, "relative_translation_max">; 
			};
		struct rotation : component<utils::math::angle::radf, "relative_rotation"> 
			{
			using component<utils::math::angle::radf, "relative_rotation">::component;
			using min = component<utils::math::angle::radf, "relative_rotation_min">;
			using max = component<utils::math::angle::radf, "relative_rotation_max">;
			};
		struct scaling : component<float, "relative_scaling"> 
			{
			using component<float, "relative_scaling">::component;
			using min = component<float, "relative_scaling_min">; 
			using max = component<float, "relative_scaling_max">; 
			};
		struct next
			{
			struct translation : component<utils::math::vec2f, "relative_next_translation"    >
				{
				using component<utils::math::vec2f, "relative_next_translation">::component;
				using current_t = relative::translation;
				};
			struct rotation : component<utils::math::angle::radf, "relative_next_rotation"> 
				{ 
				using      component<utils::math::angle::radf, "relative_next_rotation">::component;
				using current_t = relative::rotation;
				};
			struct scaling : component<float           , "relative_next_scaling"> 
				{ 
				using      component<float           , "relative_next_scaling">::component;
				using current_t = relative::scaling;
				};
			};
		};
	struct absolute
		{
		struct translation : component<utils::math::vec2f, "absolute_translation"    >
			{
			using component<utils::math::vec2f, "absolute_translation">::component;
			using min = component<utils::math::vec2f, "absolute_translation_min">;
			using max = component<utils::math::vec2f, "absolute_translation_max">;
			};
		struct rotation : component<utils::math::angle::radf, "absolute_rotation">
			{
			using component<utils::math::angle::radf, "absolute_rotation">::component;
			using min = component<utils::math::angle::radf, "absolute_rotation_min">;
			using max = component<utils::math::angle::radf, "absolute_rotation_max">;
			};
		struct scaling : component<float, "absolute_scaling">
			{
			using component<float, "absolute_scaling">::component;
			using min = component<float, "absolute_scaling_min">;
			using max = component<float, "absolute_scaling_max">;
			};
		struct next
			{
			struct translation : component<utils::math::vec2f, "absolute_next_translation"    >
				{
				using component<utils::math::vec2f, "absolute_next_translation">::component;
				using current_t = absolute::translation;
				};
			struct rotation : component<utils::math::angle::radf, "absolute_next_rotation"> 
				{ 
				using      component<utils::math::angle::radf, "absolute_next_rotation">::component;
				using current_t = absolute::rotation;
				};
			struct scaling : component<float           , "absolute_next_scaling"> 
				{ 
				using      component<float           , "absolute_next_scaling">::component;
				using current_t = absolute::scaling;
				};
			};
		};
	struct interpolated
		{
		using translation = component<utils::math::vec2f, "interpolated_translation">;
		using rotation    = component<utils::math::angle::radf, "interpolated_rotation">;
		using scaling     = component<float, "interpolated_scaling">;
		};
	struct speed
		{
		struct translation : component<utils::math::vec2f, "speed_translation"    >
			{
			using component<utils::math::vec2f, "speed_translation">::component;
			using min = component<utils::math::vec2f, "speed_translation_min">;
			using max = component<utils::math::vec2f, "speed_translation_max">;
			};
		struct directional : component<utils::math::vec2f, "speed_directional"    >
			{
			using component<utils::math::vec2f, "speed_directional">::component;
			using min = component<utils::math::vec2f, "speed_directional_min">;
			using max = component<utils::math::vec2f, "speed_directional_max">;
			};
		struct rotation : component<utils::math::angle::radf, "speed_rotation">
			{
			using component<utils::math::angle::radf, "speed_rotation">::component;
			using min = component<utils::math::angle::radf, "speed_rotation_min">;
			using max = component<utils::math::angle::radf, "speed_rotation_max">;
			};
		struct scaling : component<float, "speed_scaling">
			{
			using component<float, "speed_scaling">::component;
			using min = component<float, "speed_scaling_min">;
			using max = component<float, "speed_scaling_max">;
			};
		struct translation_magnitude
			{
			using min = component<float, "speed_translation_magnitude_min">;
			using max = component<float, "speed_translation_magnitude_max">;
			};
		};
	struct acceleration
		{
		struct translation : component<utils::math::vec2f, "acceleration_translation"    >
			{
			using component<utils::math::vec2f, "acceleration_translation">::component;
			using min = component<utils::math::vec2f, "acceleration_translation_min">;
			using max = component<utils::math::vec2f, "acceleration_translation_max">;
			};
		struct directional : component<utils::math::vec2f, "acceleration_directional"    >
			{
			using component<utils::math::vec2f, "acceleration_directional">::component;
			using min = component<utils::math::vec2f, "acceleration_directional_min">;
			using max = component<utils::math::vec2f, "acceleration_directional_max">;
			};
		struct rotation : component<utils::math::angle::radf, "acceleration_rotation">
			{
			using component<utils::math::angle::radf, "acceleration_rotation">::component;
			using min = component<utils::math::angle::radf, "acceleration_rotation_min">;
			using max = component<utils::math::angle::radf, "acceleration_rotation_max">;
			};
		struct scaling : component<float, "acceleration_scaling">
			{
			using component<float, "acceleration_scaling">::component;
			using min = component<float, "acceleration_scaling_min">;
			using max = component<float, "acceleration_scaling_max">;
			};
		struct translation_magnitude
			{
			using min = component<float, "acceleration_translation_magnitude_min">;
			using max = component<float, "acceleration_translation_magnitude_max">;
			};
		};
	struct moved
		{
		static constexpr auto in_place_delete = true;
		};


	// Transform types
	template <typename T>
	concept is_absolute     = ::utils::concepts::any_of_undecorated<T, absolute::translation    , absolute::next::translation, absolute::rotation, absolute::next::rotation, absolute::scaling, absolute::next::scaling>;
	template <typename T>
	concept is_relative     = ::utils::concepts::any_of_undecorated<T, relative::translation    , relative::next::translation, relative::rotation, relative::next::rotation, relative::scaling, relative::next::scaling>;
	template <typename T>
	concept is_speed        = ::utils::concepts::any_of_undecorated<T, speed::translation       , speed::directional, speed::rotation, speed::scaling>;
	template <typename T>
	concept is_acceleration = ::utils::concepts::any_of_undecorated<T, acceleration::translation, acceleration::directional, acceleration::rotation, acceleration::scaling>;
	template <typename T>
	concept is_interpolated = ::utils::concepts::any_of_undecorated<T, interpolated::translation, interpolated::rotation, interpolated::scaling>;

	// Transform component types
	template <typename T>
	concept is_translation = ::utils::concepts::any_of_undecorated<T, absolute::translation, absolute::next::translation, relative::translation, relative::next::translation, speed::translation, acceleration::translation, interpolated::translation>;
	template <typename T>
	concept is_directional = ::utils::concepts::any_of_undecorated<T, speed::directional   , acceleration::directional>;
	template <typename T>
	concept is_rotation    = ::utils::concepts::any_of_undecorated<T, absolute::rotation   , absolute::next::rotation, relative::rotation, relative::next::rotation, speed::rotation, acceleration::rotation, interpolated::rotation>;
	template <typename T>
	concept is_scaling     = ::utils::concepts::any_of_undecorated<T, absolute::scaling    , absolute::next::scaling, relative::scaling, relative::next::scaling, speed::scaling, acceleration::scaling, interpolated::scaling>;

	template <typename T>
	concept is_next = ::utils::concepts::any_of_undecorated
		<T, 
		absolute::next::translation, absolute::next::rotation, absolute::next::scaling, 
		relative::next::translation, relative::next::rotation, relative::next::scaling
		>;

	template <typename T>
	concept is_transform_component = is_translation<T> || is_rotation<T> || is_scaling<T>;
	}

namespace iige::ecs::components
	{
	struct parent
		{
		size_t children_count{0};
		entt::entity first_child{entt::null};
		};
	struct child
		{
		entt::entity parent{entt::null};
		entt::entity next_sibling{entt::null};
		entt::entity prev_sibling{entt::null};
		};
	}