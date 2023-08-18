#pragma once

#include <utils/variadic.h>

#include "ecs/entt.h"
#include "ecs/components/spatial.h"

namespace iige
	{
	class scene
		{
		friend class entity;
		public:
			entt::entity create_entity() {  }
			entt::entity create_entity(utils::math::transform2 transform)
				{
				entt::entity identifier{ecs_registry.create()};
				ecs_registry.emplace<iige::ecs::components::transform::absolute::translation>(identifier, transform.translation);
				ecs_registry.emplace<iige::ecs::components::transform::absolute::rotation   >(identifier, transform.rotation);
				ecs_registry.emplace<iige::ecs::components::transform::absolute::scaling    >(identifier, transform.scaling);
				return identifier;
				}

			entt::registry ecs_registry;

			template<typename ... component_ts, typename... excluded_ts>
			[[nodiscard]] entt::basic_view<entt::registry::entity_type, entt::get_t<component_ts...>, entt::exclude_t<excluded_ts...>> view(entt::exclude_t<excluded_ts...> = {})
				{
				// TODO do these checks in the parameters of the .each lambda called in our view wrapper, not here.
				static_assert (
					(((iige::ecs::components::transform::is_absolute<component_ts> && !iige::ecs::components::transform::is_next<component_ts>) ? std::is_const_v<component_ts> : true) && ...) ||
					(((iige::ecs::components::transform::is_relative<component_ts> && !iige::ecs::components::transform::is_next<component_ts>) ? std::is_const_v<component_ts> : true) && ...),
					"Current transform state cannot be changed manually. Query transform componenets as const. If you want to modify any transform component, query [transform]::next::[component] instead."
					);
					

				return ecs_registry.view<component_ts..., excluded_ts...>();
				}

			template <typename component_t, typename ... arg_ts>
			component_t& emplace(entt::entity entity, arg_ts&& ... args)
				{
				return ecs_registry.emplace<component_t>(entity, std::forward<arg_ts>(args)...);
				}
			template <typename component_t, typename ... arg_ts>
			component_t& remove(entt::entity entity)
				{
				return ecs_registry.remove<component_t>(entity);
				}
		private:
			//utils::polymorphic_container<batch_container, Object, Ts...> inh_container;
		};
	}