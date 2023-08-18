#pragma once

#include <vector>
#include <functional>
#include <chrono>

#include <utils/clock.h>
#include <utils/memory.h>
#include <utils/logger.h>

#include "loop.h"
#include "systems_manager.h"

#include "window.h"

namespace iige
	{
	class window_loop_interop : public loop::actions_interface
		{
		public:
			window_loop_interop(iige::window& window, iige::systems_manager& systems_manager) :
				window{window}, systems_manager{systems_manager} {}

			bool is_running() const noexcept { return window.get().is_open(); }

			void step(float delta_time)
				{
				iige::window& window{this->window.get()};
				iige::systems_manager& systems_manager{this->systems_manager.get()};

				while (window.poll_event()) {}

				for (const auto& step_system : systems_manager.step)
					{
					step_system(delta_time);
					}
				}
			void draw(float delta_time, float interpolation)
				{
				iige::window& window{this->window.get()};
				iige::systems_manager& systems_manager{this->systems_manager.get()};

				for (const auto& pre_draw_system : systems_manager.pre_draw)
					{
					pre_draw_system(delta_time, interpolation);
					}

				window.render_module_ptr->draw([&](const utils::MS::window::base&, const utils::MS::graphics::d2d::device_context& context)
					{
					context->SetTransform(D2D1::IdentityMatrix());
					context->Clear(D2D1_COLOR_F{0.f, 0.f, 0.f, 0.f});

					for (const auto& draw_system : systems_manager.draw)
						{
						draw_system(delta_time, interpolation);
						}
					});
				}

		private:
			std::reference_wrapper<iige::window         > window;
			std::reference_wrapper<iige::systems_manager> systems_manager;
		};
	}