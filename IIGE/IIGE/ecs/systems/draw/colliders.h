#pragma once

#include <utility>
#include <mutex>
#include <execution>

#include <utils/math/geometry/interactions.h>
#include <utils/math/geometry/continuous_interactions.h>
#include <utils/variant.h>
#include <utils/math/angle.h>

#include "../../../to_sf.h"

#include "../../entt.h"

#include "../../components/collision.h"
#include "../../components/spatial.h"
#include "../../../types/core.h"

#include "../../../window.h"
#include "../../../scene.h"

#include "../system.h"

namespace iige::ecs::systems::draw
	{
	class colliders : public systems::system_step, public systems::system_draw
		{
		public:
			virtual void step(iige::scene& scene, iige::window&, float) override
				{
				update_colliders_vertex_array(scene);
				}

			virtual void draw(iige::scene&, iige::window& window, float, float) override
				{
				window.sf_window.draw(va);
				}

		private:
			sf::VertexArray va{sf::PrimitiveType::Lines, 3600};

			void update_colliders_vertex_array(const shapes::segment& shape, sf::Color c)
				{
				va.append(sf::Vertex{{shape.a.x, shape.a.y}, c}); va.append(sf::Vertex{{shape.b.x, shape.b.y}, c});
				}
			void update_colliders_vertex_array(const shapes::circle& shape, sf::Color c)
				{
				iige::vec2f right{iige::vec2f::right() * shape.radius};
				iige::vec2f vec{iige::vec2f::right() * shape.radius};

				utils::math::angle::deg delta_α{1000 / shape.radius};

				using namespace utils::math::angle::literals;
				for (utils::math::angle::deg α = delta_α; α.value < utils::math::angle::deg::full_angle; α += delta_α)
					{
					va.append(sf::Vertex{iige::to_sf<float>(vec + shape.center), c});
					vec = right + α;
					va.append(sf::Vertex{iige::to_sf<float>(vec + shape.center), c});
					}
				va.append(sf::Vertex{iige::to_sf<float>(vec + shape.center), c});
				va.append(sf::Vertex{iige::to_sf<float>(right + shape.center), c});
				}
			void update_colliders_vertex_array(const shapes::polygon& shape, sf::Color c)
				{
				for (const auto& edge : shape.get_edges())
					{
					va.append(sf::Vertex{iige::to_sf<float>(edge.a), c});
					va.append(sf::Vertex{iige::to_sf<float>(edge.b), c});
					}
				}
			void update_colliders_vertex_array(const shapes::aabb& shape, sf::Color c)
				{
				va.append(sf::Vertex{iige::to_sf<float>(shape.ul), c});
				va.append(sf::Vertex{iige::to_sf<float>(shape.ur), c});
				va.append(sf::Vertex{iige::to_sf<float>(shape.ur), c});
				va.append(sf::Vertex{iige::to_sf<float>(shape.dr), c});
				va.append(sf::Vertex{iige::to_sf<float>(shape.dr), c});
				va.append(sf::Vertex{iige::to_sf<float>(shape.dl), c});
				va.append(sf::Vertex{iige::to_sf<float>(shape.dl), c});
				va.append(sf::Vertex{iige::to_sf<float>(shape.ul), c});
				}
			void update_colliders_vertex_array(const shapes::point& shape, sf::Color c)
				{
				shapes::aabb aabb{.ll{shape.x - 1}, .up{shape.y - 1}, .rr{shape.x + 1}, .dw{shape.y + 1}};
				update_colliders_vertex_array(aabb, c);
				}
			void update_colliders_vertex_array(const utils::math::geometry::continuous_point& shape, sf::Color c)
				{
				update_colliders_vertex_array(shape.a, c);
				update_colliders_vertex_array(static_cast<shapes::segment>(shape), c);

				vec2f arrow_hand_ll{shape.b + (shape.perpendicular_left() * 4.f) - (shape.forward() * 4.f)};
				vec2f arrow_hand_rr{shape.b + (shape.perpendicular_right() * 4.f) - (shape.forward() * 4.f)};
				update_colliders_vertex_array(shapes::segment{shape.b, arrow_hand_ll}, c);
				update_colliders_vertex_array(shapes::segment{shape.b, arrow_hand_rr}, c);
				}

			template <components::colliders::is_collider T>
			void update_colliders_vertex_array(iige::scene& scene)
				{
				auto colliders_not_colliding{scene.ecs_registry.view<T>(entt::exclude<components::collided_with>)};
				auto colliders_____colliding{scene.ecs_registry.view<T, components::collided_with>()};

				sf::Color c;

				c = sf::Color::White;
				colliders_not_colliding.each([&](entt::entity entity, const T& shape)
					{
					update_colliders_vertex_array(shape.value(), c);
					});
				c = sf::Color::Red;
				colliders_____colliding.each([&](entt::entity entity, const T& shape, components::collided_with)
					{
					update_colliders_vertex_array(shape.value(), c);
					});
				}

			sf::VertexArray& update_colliders_vertex_array(iige::scene& scene)
				{
				va.clear();

				update_colliders_vertex_array<components::colliders::point                 >(scene);
				update_colliders_vertex_array<components::colliders::segment               >(scene);
				update_colliders_vertex_array<components::colliders::aabb                  >(scene);
				update_colliders_vertex_array<components::colliders::circle                >(scene);
				update_colliders_vertex_array<components::colliders::polygon               >(scene);
				update_colliders_vertex_array<components::colliders::convex_polygon        >(scene);
				update_colliders_vertex_array<components::colliders::hollow_aabb           >(scene);
				update_colliders_vertex_array<components::colliders::hollow_circle         >(scene);
				update_colliders_vertex_array<components::colliders::hollow_polygon        >(scene);
				update_colliders_vertex_array<components::colliders::hollow_convex_polygon >(scene);
				update_colliders_vertex_array<components::colliders::continuous_point      >(scene);

				//update_colliders_vertex_array<components::colliders::details::bounding_aabb>(scene, va);

				return va;
				}
		};
	}