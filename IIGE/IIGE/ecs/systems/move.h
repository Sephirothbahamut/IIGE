#pragma once

#include <string>
#include <execution>
#include <stdexcept>

#include <utils/math/geometry/shapes.h>
#include <utils/compilation/debug.h>

#include "../entt.h"

#include "../../scene.h"

#include "../components/bad_draw.h"
#include "../components/spatial.h"
#include "../components/collision.h"

namespace iige::ecs::systems
	{
	template <components::colliders::is_collider collider_t>
	void move_colliders(iige::scene& scene)
		{
		auto reset{scene.ecs_registry.view<typename collider_t::discrete_source, collider_t>()};
		reset.each([](const collider_t::discrete_source& collider_source, collider_t& collider) 
			{
			if constexpr (components::colliders::is_discrete_collider<collider_t>)
				{
				collider.value() = collider_source.value();
				}
			//else if constexpr (components::colliders::is_continuous_collider<collider_t>)
			//	{
			//	collider.value().a = collider_source.value();
			//	collider.value().b = collider_source.value();
			//	}
			});
		
		if constexpr (components::colliders::is_discrete_collider<collider_t>)
			{
			auto scaling{scene.ecs_registry.view<components::transform::absolute::scaling, collider_t>()};
			scaling.each([](const float& scaling, collider_t& collider)
				{
				collider.value().scale_self(scaling);
				});
			auto rotating{scene.ecs_registry.view<components::transform::absolute::rotation, collider_t>()};
			rotating.each([](const utils::math::angle::radf& rotation, collider_t& collider)
				{
				collider.value().rotate_self(rotation);
				});
			auto translating{scene.ecs_registry.view<components::transform::absolute::translation, collider_t>()};
			translating.each([](const utils::math::vec2f& xy, collider_t& collider)
				{
				collider.value().translate_self(xy);
				});
			}
		//else if constexpr (components::colliders::is_continuous_collider<collider_t>)
		//	{
		//	auto scaling{scene.ecs_registry.view<components::transform::absolute::scaling, components::transform::absolute::next::scaling, collider_t>()};
		//	scaling.each([](const float& scaling, const float& scaling_next, collider_t& collider)
		//		{
		//		collider.value().scale_self(scaling, scaling_next);
		//		});
		//	auto rotating{scene.ecs_registry.view<components::transform::absolute::rotation, components::transform::absolute::next::rotation, collider_t>()};
		//	rotating.each([](const utils::math::angle::radf& rotation, const utils::math::angle::radf& rotation_next, collider_t& collider)
		//		{
		//		collider.value().rotate_self(rotation, rotation_next);
		//		});
		//	auto translating{scene.ecs_registry.view<components::transform::absolute::translation, components::transform::absolute::next::translation, collider_t>()};
		//	translating.each([](const utils::math::vec2f& xy, const utils::math::vec2f& xy_next, collider_t& collider)
		//		{
		//		collider.value().translate_selfxy, xy_next);
		//		});
		//	}

		auto moved{scene.ecs_registry.view<collider_t, components::colliders::details::bounding_aabb, components::transform::moved>()};
		moved.each([](const collider_t& collider, components::colliders::details::bounding_aabb& aabb) 
			{
			aabb.value() = collider.value().bounding_box();
			});
		}

	namespace details
		{
		template <typename T, typename constraint_t = T>
		inline void apply_constaints_inner(iige::scene& scene)
			{
			auto clamp{scene.ecs_registry.view<T, typename constraint_t::min, typename constraint_t::max>()};
			clamp.each([](T::value_type& value, const constraint_t::min::value_type& min, const constraint_t::max::value_type& max)
				{
				value = utils::math::clamp(value, min, max);
				});

			if constexpr (std::is_same_v<typename T::value_type, float>) // No min or max alone with angles doesn't make sense.
				{
				auto min{scene.ecs_registry.view<T, typename constraint_t::min>(entt::exclude<typename constraint_t::max>)};
				min.each([](T::value_type& value, const constraint_t::min::value_type& min) { value = std::max(value, min); });

				auto max{scene.ecs_registry.view<T, typename constraint_t::max>(entt::exclude<typename constraint_t::min>)};
				max.each([](T::value_type& value, const constraint_t::max::value_type& max) { value = std::min(value, max); });
				}
			}
		template <typename T, typename constraint_t = T>
		inline void apply_constraints(iige::scene& scene)
			{
			apply_constaints_inner<typename T::translation, typename constraint_t::translation>(scene);
			apply_constaints_inner<typename T::rotation   , typename constraint_t::rotation   >(scene);
			apply_constaints_inner<typename T::scaling    , typename constraint_t::scaling    >(scene);
			}

		template <typename T>
		inline void apply_constraints_directional(iige::scene& scene)
			{
			apply_constaints_inner<typename T::directional>(scene);
			}

		template <typename T>
		inline void apply_constraints_magnitude(iige::scene& scene)
			{
			auto clamp{scene.ecs_registry.view<typename T::translation, typename T::translation_magnitude::min, typename T::translation_magnitude::max>()};
			clamp.each([](T::translation::value_type& xy, const T::translation_magnitude::min::value_type& min, const T::translation_magnitude::max::value_type& max)
				{
				xy.length = utils::math::clamp(xy.length, min, max);
				});
			
			auto min{scene.ecs_registry.view<typename T::translation, typename T::translation_magnitude::min>(entt::exclude<typename T::translation_magnitude::max>)};
			min.each([](T::translation::value_type& xy, const T::translation_magnitude::min::value_type& min)
				{
				xy.length = std::max(xy.length, min);
				});
			
			auto max{scene.ecs_registry.view<typename T::translation, typename T::translation_magnitude::max>(entt::exclude<typename T::translation_magnitude::min>)};
			max.each([](T::translation::value_type& xy, const T::translation_magnitude::max::value_type& max)
				{
				xy.length = std::min(xy.length, max);
				});
			}

		template <typename self_t, typename other_t>
		inline void add_to_self_inner(iige::scene& scene, float delta_time)
			{
			auto view{scene.ecs_registry.view<self_t, other_t>()};
			view.each([delta_time](self_t::value_type& self, const other_t::value_type& other) { self += other * delta_time; });
			}
		template <typename self_t, typename other_t, typename exclude_t>
		inline void add_to_self_exclude_inner(iige::scene& scene, float delta_time)
			{
			auto view{scene.ecs_registry.view<self_t, other_t>(entt::exclude<exclude_t>)};
			view.each([delta_time](self_t::value_type& self, const other_t::value_type& other) { self += other * delta_time; });
			}
		template <typename from_t, typename to_t>
		inline void assign_to_inner(iige::scene& scene)
			{
			auto view{scene.ecs_registry.view<from_t, to_t>()};
			view.each([](const from_t::value_type& from, to_t::value_type& to) { to = from; });
			}

		template <typename self_t, typename other_t>
		inline void add_to_self(iige::scene& scene, float delta_time)
			{
			add_to_self_inner<typename self_t::translation, typename other_t::translation>(scene, delta_time);
			add_to_self_inner<typename self_t::rotation   , typename other_t::rotation   >(scene, delta_time);
			add_to_self_inner<typename self_t::scaling    , typename other_t::scaling    >(scene, delta_time);
			}


		template <typename self_t, typename directional_t>
		inline void add_direcitonal_to_self(iige::scene& scene, float delta_time)
			{
			auto abs{scene.ecs_registry.view<typename self_t::translation, typename directional_t::directional, components::transform::absolute::rotation>(entt::exclude<components::transform::relative::rotation>)};

			auto lambda{[delta_time](utils::math::vec2f& translation, const utils::math::vec2f& directional, const utils::math::angle::radf& rotation)
				{
				translation += (directional - rotation) * delta_time;
				}};

			abs.each(lambda);

			auto rel{scene.ecs_registry.view<typename self_t::translation, typename directional_t::directional, components::transform::relative::rotation>()};

			rel.each(lambda);
			}

		template <typename self_t, typename out_t, typename exclude_t>
		inline void add_to_self_exclude(iige::scene& scene, float delta_time)
			{
			add_to_self_exclude_inner<typename self_t::translation, typename out_t::translation, typename exclude_t::translation>(scene, delta_time);
			add_to_self_exclude_inner<typename self_t::rotation   , typename out_t::rotation   , typename exclude_t::rotation   >(scene, delta_time);
			add_to_self_exclude_inner<typename self_t::scaling    , typename out_t::scaling    , typename exclude_t::scaling    >(scene, delta_time);
			}

		template <typename self_t, typename directional_t, typename exclude_t>
		inline void add_direcitonal_to_self_exclude(iige::scene& scene, float delta_time)
			{
			auto abs{scene.ecs_registry.view<typename self_t::translation, typename directional_t::directional, components::transform::absolute::rotation>(entt::exclude<components::transform::relative::rotation, typename exclude_t::translation>)};

			auto lambda{[delta_time](utils::math::vec2f& translation, const utils::math::vec2f& directional, const utils::math::angle::radf& rotation)
				{
				translation += (directional - rotation) * delta_time;
				}};

			abs.each(lambda);

			auto rel{scene.ecs_registry.view<typename self_t::translation, typename directional_t::directional, components::transform::relative::rotation>(entt::exclude<typename exclude_t::translation>)};

			rel.each(lambda);
			}

		template <typename from_t, typename to_t>
		inline void assign_to(iige::scene& scene)
			{
			assign_to_inner<typename from_t::translation, typename to_t::translation>(scene);
			assign_to_inner<typename from_t::rotation, typename to_t::rotation   >(scene);
			assign_to_inner<typename from_t::scaling    , typename to_t::scaling    >(scene);
			}

		template <typename from_t, typename to_t, typename out_t>
		inline void interpolate_inner(iige::scene& scene, float interpolation)
			{
			auto view{scene.ecs_registry.view<from_t, to_t, out_t>()};
			view.each([interpolation](const from_t::value_type& from, const to_t::value_type& to, out_t::value_type& out)
				{
				out = utils::math::lerp(from, to, interpolation);
				});
			}

		template <typename T>
		inline void mark_moved_inner(iige::scene& scene)
			{
			auto moved{scene.ecs_registry.view<T>()};
			moved.each([&scene](entt::entity entity, T) { scene.ecs_registry.get_or_emplace<components::transform::moved>(entity); });
			}
		inline void mark_moved(iige::scene& scene)
			{
			mark_moved_inner<components::transform::absolute::next::translation>(scene);
			mark_moved_inner<components::transform::absolute::next::rotation   >(scene);
			mark_moved_inner<components::transform::absolute::next::scaling    >(scene);
			}


		template <components::transform::is_transform_component T>
		inline utils::observer_ptr<typename T::value_type> get_value_next_if_present_otherwise_current(iige::scene& scene, entt::entity entity)
			{
			if (auto ptr{scene.ecs_registry.try_get<T>(entity)})
				{
				return &(ptr->value());
				}

			if constexpr (components::transform::is_next<T>)
				{
				if (auto ptr{scene.ecs_registry.try_get<typename T::current_t>(entity)})
					{
					return &(ptr->value());
					}
				}

			return nullptr;
			}

		template <typename relative_t, typename absolute_t, typename base_t>
		inline void add_parent_inner(iige::scene& scene)
			{
			auto view{scene.ecs_registry.view<relative_t, absolute_t>()};
			view.each([&](entt::entity entity, const relative_t::value_type& relative, absolute_t::value_type& absolute)
				{
				if constexpr (utils::compilation::debug)
					{
					//TODO make sure this is guaranteed by the scene
					if (!scene.ecs_registry.all_of<components::child>(entity))
						{
						throw std::runtime_error{"Malformed hierarchy structure: entity \"" + std::to_string(static_cast<uint32_t>(entity)) + "\" has relative transform components, but lacks child component."};
						}
					}
				auto parent_id{scene.ecs_registry.get<components::child>(entity).parent};

				auto updated_relative{relative};

				if constexpr (components::transform::is_translation<relative_t>)
					{
					auto parent_scaling_ptr{get_value_next_if_present_otherwise_current<typename base_t::scaling>(scene, parent_id)};
					if (parent_scaling_ptr) { updated_relative *= *parent_scaling_ptr; }

					auto parent_rotation_ptr{get_value_next_if_present_otherwise_current<typename base_t::rotation>(scene, parent_id)};
					if (parent_rotation_ptr) { updated_relative += *parent_rotation_ptr; }
					}

				auto parent_value_ptr{get_value_next_if_present_otherwise_current<absolute_t>(scene, parent_id)};
				if (parent_value_ptr)
					{
					absolute = updated_relative + *parent_value_ptr;
					}
				else
					{
					absolute = updated_relative;
					}
				});
			}
		inline void add_parent(iige::scene& scene)
			{
			// TODO sort only if moved ? and if parent moved ???
			scene.ecs_registry.sort<components::child>([&](const entt::entity lhs, const entt::entity rhs) {
				const auto& clhs = scene.ecs_registry.get<components::child>(lhs);
				const auto& crhs = scene.ecs_registry.get<components::child>(rhs);
				return crhs.parent == lhs || clhs.next_sibling == rhs
					|| (!(clhs.parent == rhs || crhs.next_sibling == lhs) && (clhs.parent < crhs.parent || (clhs.parent == crhs.parent && &clhs < &crhs)));
				});

			add_parent_inner<components::transform::relative      ::translation, components::transform::absolute      ::translation, components::transform::absolute      >(scene);
			add_parent_inner<components::transform::relative      ::rotation   , components::transform::absolute      ::rotation   , components::transform::absolute      >(scene);
			add_parent_inner<components::transform::relative      ::scaling    , components::transform::absolute      ::scaling    , components::transform::absolute      >(scene);
			add_parent_inner<components::transform::relative::next::translation, components::transform::absolute::next::translation, components::transform::absolute::next>(scene);
			add_parent_inner<components::transform::relative::next::rotation   , components::transform::absolute::next::rotation   , components::transform::absolute::next>(scene);
			add_parent_inner<components::transform::relative::next::scaling    , components::transform::absolute::next::scaling    , components::transform::absolute::next>(scene);
			}

		template <typename remove_t, typename ...if_not_t>
		inline void clear_teleports_inner_inner(iige::scene& scene)
			{
			auto view{scene.ecs_registry.view<remove_t>(entt::exclude<if_not_t...>)};
			view.each([&](entt::entity entity, const remove_t&) { scene.ecs_registry.remove<remove_t>(entity); });
			}
		template <typename T>
		inline void clear_teleports_inner(iige::scene& scene)
			{
			clear_teleports_inner_inner<typename T::next::translation, components::transform::speed::translation, components::transform::speed::directional>(scene);
			clear_teleports_inner_inner<typename T::next::rotation   , components::transform::speed::rotation   >(scene);
			clear_teleports_inner_inner<typename T::next::scaling    , components::transform::speed::scaling    >(scene);
			}
		template <typename T>
		inline void clear_teleports(iige::scene& scene)
			{
			clear_teleports_inner<components::transform::absolute>(scene);
			clear_teleports_inner<components::transform::relative>(scene);
			}
		}


	inline void move(iige::scene& scene, float delta_time)
		{
		scene.ecs_registry.clear<components::transform::moved>();
		details::mark_moved(scene);

		details::assign_to<components::transform::relative::next, components::transform::relative>(scene);
		details::assign_to<components::transform::absolute::next, components::transform::absolute>(scene);

		// Speed and acceleration
		details::apply_constraints            <components::transform::acceleration                              >(scene);
		details::apply_constraints_directional<components::transform::acceleration                              >(scene);
		details::apply_constraints_magnitude  <components::transform::acceleration                              >(scene);
		details::add_to_self                  <components::transform::speed, components::transform::acceleration>(scene, delta_time);
		details::add_direcitonal_to_self      <components::transform::speed, components::transform::acceleration>(scene, delta_time);
		details::apply_constraints            <components::transform::speed                                     >(scene);
		details::apply_constraints_directional<components::transform::speed                                     >(scene);
		details::apply_constraints_magnitude  <components::transform::speed                                     >(scene);
		
		// Relative movement
		
		details::add_to_self            <components::transform::relative::next, components::transform::speed>(scene, delta_time);
		details::add_direcitonal_to_self<components::transform::relative::next, components::transform::speed>(scene, delta_time);

		details::apply_constraints      <components::transform::relative::next, components::transform::relative>(scene);
		
		// Absolute only movement
		// Exclude relative > evaluates movement for entities without a parent
		details::add_to_self_exclude            <components::transform::absolute::next, components::transform::speed, components::transform::relative>(scene, delta_time);
		details::add_direcitonal_to_self_exclude<components::transform::absolute::next, components::transform::speed, components::transform::relative>(scene, delta_time);

		// Absolute based on relative
		details::add_parent                                                                                                                                                 (scene);

		details::clear_teleports<components::transform::absolute                                                                                                           >(scene);
		details::clear_teleports<components::transform::relative                                                                                                           >(scene);

		
		details::apply_constraints<components::transform::absolute::next, components::transform::absolute                                                                  >(scene);
		
		move_colliders<components::colliders::point         >(scene);
		move_colliders<components::colliders::segment       >(scene);
		move_colliders<components::colliders::aabb          >(scene);
		move_colliders<components::colliders::circle        >(scene);
		move_colliders<components::colliders::polygon       >(scene);
		//move_colliders<components::colliders::convex_polygon>(scene);
		}

	inline void interpolate(iige::scene& scene, float interpolation)
		{
		details::interpolate_inner<components::transform::absolute::translation, components::transform::absolute::next::translation, components::transform::interpolated::translation>(scene, interpolation);
		details::interpolate_inner<components::transform::absolute::rotation   , components::transform::absolute::next::rotation   , components::transform::interpolated::rotation   >(scene, interpolation);
		details::interpolate_inner<components::transform::absolute::scaling    , components::transform::absolute::next::scaling    , components::transform::interpolated::scaling    >(scene, interpolation);
		}
	}