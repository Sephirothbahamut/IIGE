#pragma once
#include <bitset>

namespace iige { class scene; class window; }

namespace iige::ecs::systems
	{
	struct system_step { virtual void step(iige::scene&, iige::window&, float       ) = 0; };
	struct system_draw { virtual void draw(iige::scene&, iige::window&, float, float) = 0; };
	}