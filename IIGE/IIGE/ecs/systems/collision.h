#pragma once

#include <utility>
#include <mutex>
#include <execution>

#include <utils/math/geometry/interactions.h>
//#include <utils/math/geometry/continuous_interactions.h>
#include <utils/variant.h>
#include <utils/math/angle.h>

#include "../entt.h"

#include "../components/collision.h"
#include "../components/spatial.h"

#include "../../window.h"
#include "../../scene.h"



namespace iige::ecs::systems
	{
	class collision
		{
		public:
			virtual void operator()(iige::scene& scene) const noexcept = 0;
		};

	template <size_t layers_count>
	class collision_impl : public collision
		{
		public:
			virtual void operator()(iige::scene& scene) const noexcept final override
				{
				scene.ecs_registry.clear<components::collided_with>();
				scene.ecs_registry.clear<components::collision_data>();

				[&] <std::size_t... I>(std::index_sequence<I...>) {
					(evaluate<I>(scene), ...);
					}(std::make_index_sequence<layers_count>{});
				}
		private:
#pragma optimize("", off)
			template<size_t layer>
			void evaluate(iige::scene& scene) const noexcept
				{
				auto targets{scene.ecs_registry.view<components::has_collision<layer>, components::colliders::details::bounding_aabb, components::colliders::details::ptr>()};
				auto active {scene.ecs_registry.view<components::collides_with<layer>, components::colliders::details::bounding_aabb, components::colliders::details::ptr>()};
				
				/*
				std::mutex adding_mutex;
				std::for_each(std::execution::par, active.begin(), active.end(), [&](const entt::entity entity_a)
					{
					const components::colliders::aabb&  a_aabb        {active.get<components::colliders::aabb        >(entity_a)};
					components::colliders::details::ptr a_collider_ptr{active.get<components::colliders::details::ptr>(entity_a)};
					/*/
				active.each([&](const entt::entity entity_a, const components::colliders::details::bounding_aabb& a_aabb, components::colliders::details::ptr a_collider_ptr)
					{/**/
					targets.each([&](const entt::entity entity_b, const components::colliders::details::bounding_aabb& b_aabb, components::colliders::details::ptr b_collider_ptr)
						{
						if (entity_a == entity_b) { return; }
				
						if (a_aabb.value().collides_with(b_aabb.value()))
							{
							bool collides{false};
				
							std::visit([&](const auto& a_collider_ptr)
								{
								std::visit([&](const auto& b_collider_ptr)
									{
									const auto& a_collider {*a_collider_ptr};
									const auto& b_collider {*b_collider_ptr};
									const auto& a_shape    { a_collider_ptr->value()};
									const auto& b_shape    { b_collider_ptr->value()};
									using a_collider_t     = std::remove_reference<decltype(a_collider)>::type;
									using b_collider_t     = std::remove_reference<decltype(b_collider)>::type;
									using a_shape_t        = std::remove_reference<decltype(a_shape   )>::type;
									using b_shape_t        = std::remove_reference<decltype(b_shape   )>::type;
									
									if constexpr (components::colliders::is_discrete_collider<a_collider_t>)
										{
										//bool collides{a_shape.collides_with(b_shape)};
										//if (collides)
										//	{
										//	//std::unique_lock lock{adding_mutex};
										//	std::ignore(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b));
										//	}
										}
									//else if constexpr (components::colliders::is_continuous_collider<a_collider_t>)
									//	{
									//	bool inside{utils::math::geometry::contains(b_shape, a_shape.a)};
									//
									//	auto result{utils::math::geometry::continuous_collides(a_shape, b_shape, inside)};
									//	if (result)
									//		{
									//		//std::unique_lock lock{adding_mutex};
									//		std::ignore(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b));
									//		std::ignore(scene.ecs_registry.get_or_emplace<components::collision_data>(entity_a, components::collision_data{entity_b, result.value()}));
									//		}
									//	else if constexpr (!b_collider_t::hollow)
									//		{
									//		if (inside)
									//			{
									//			//std::unique_lock lock{adding_mutex};
									//			std::ignore(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b));
									//			}
									//		}
									//	}
									}, b_collider_ptr);
								}, a_collider_ptr);
							}
						});
					});
				}
#pragma optimize("", on)
		};
	}