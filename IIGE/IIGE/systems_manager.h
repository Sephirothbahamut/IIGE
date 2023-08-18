#pragma once

#include <functional>

#include <utils/containers/object_pool.h>

namespace iige
	{
	class scene;
	class window;

	class systems_manager
		{
		using callable_step_t = std::function<void(float)>;
		using callable_draw_t = std::function<void(float, float)>;
		using container_step_t = utils::containers::object_pool<callable_step_t>;
		using container_draw_t = utils::containers::object_pool<callable_draw_t>;

		public:
			container_step_t step;
			container_draw_t draw;
			container_draw_t pre_draw;

		private:
		};
	}