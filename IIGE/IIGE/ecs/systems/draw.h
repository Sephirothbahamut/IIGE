#pragma once

#include <utils/math/vec2.h>

#include "../entt.h"

#include "../../window.h"
#include "../../scene.h"

#include "../components/bad_draw.h"
#include "../components/spatial.h"

namespace iige::ecs::systems
	{
	void bad_draw(iige::scene& scene, iige::window& window, float delta_time, float interpolation)
		{
		//TODO figure out compound transform view :|

		scene.ecs_registry.view<components::transform::interpolated::translation, components::bad_draw>()
			.each([](const vec2f& translation, auto& drawable)
				{
				drawable.cs.setPosition({translation.x, translation.y});
				});
		scene.ecs_registry.view<components::transform::absolute::translation, components::bad_draw>(entt::exclude<components::transform::interpolated::translation>)
			.each([](const vec2f& translation, auto& drawable)
				{
				drawable.cs.setPosition({translation.x, translation.y});
				});

		scene.ecs_registry.view<components::transform::interpolated::rotation, components::bad_draw>()
			.each([](const angle::rad& rotation, auto& drawable)
				{
				drawable.cs.setRotation(angle::deg(rotation).value);
				});
		scene.ecs_registry.view<components::transform::absolute::rotation, components::bad_draw>(entt::exclude< components::transform::interpolated::rotation>)
			.each([](const angle::rad& rotation, auto& drawable)
				{
				drawable.cs.setRotation(angle::deg(rotation).value);
				});

		scene.ecs_registry.view<components::transform::interpolated::scaling, components::bad_draw>()
			.each([](const float& scaling, auto& drawable)
				{
				drawable.cs.setScale({scaling, scaling});
				});
		scene.ecs_registry.view<components::transform::absolute::scaling, components::bad_draw>(entt::exclude< components::transform::interpolated::scaling>)
			.each([](const float& scaling, auto& drawable)
				{
				drawable.cs.setScale({scaling, scaling});
				});

		scene.ecs_registry.view<components::bad_draw>()
			.each([&window](components::bad_draw& drawable)
				{
				window.sf_window.draw(drawable.cs);
				});

		}
	}
