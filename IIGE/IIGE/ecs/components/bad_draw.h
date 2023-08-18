#pragma once

#include <utils/MS/graphics/d2d.h>
#include <utils/math/rect.h>

namespace iige::ecs::components
	{
	struct bad_draw
		{
		bad_draw(float size) : shape{.ll{0.f}, .up{0.f}, .rr{size}, .dw{size}} {}

		utils::math::rect<float> shape;
		};
	}