#include <utils/MS/window/window.h>

#include "IIGE/loop.h"
#include "IIGE/scene.h"
#include "IIGE/systems_manager.h"
#include "IIGE/window_loop_interop.h"

#include "IIGE/window.h"

#include "IIGE/ecs/systems/move.h"
#include "IIGE/ecs/systems/collision.h"

int main()
	{
	utils::MS::graphics::initializer graphics_initializer;

	utils::MS::window::initializer window_initializer;

	iige::scene scene;
	iige::window window{iige::window::create_info
		{
		.base{.title{L"test game"}},
		.graphics{graphics_initializer}
		}};


	iige::systems_manager systems_manager;
	systems_manager.step.emplace([&scene](float delta_time)
		{
		iige::ecs::systems::move(scene, delta_time);

		iige::ecs::systems::collision_impl<1> collision;
		collision(scene);
		});
	systems_manager.draw.emplace([&scene, &window](float delta_time, float interpolation)
		{
		auto& context{window.render_module_ptr->d2d_device_context};
		context->SetTransform(D2D1::IdentityMatrix());

		static float t{0};
		t += delta_time;
		utils::math::angle::degf hue{static_cast<float>(static_cast<size_t>(t) % 360)};

		utils::graphics::colour::hsv<float, false> hsv{.h{hue}, .s{1.f}, .v{1.f}};
		auto rgb{hsv.rgb()};

		context->Clear(D2D1_COLOR_F{rgb.r, rgb.g, rgb.b, 0.5f});
		});

	window.show();

	iige::window_loop_interop                       window_loop_interop{window, systems_manager};
	iige::loop::fixed_game_speed_variable_framerate loop               {window_loop_interop, 10.f};

	loop.run();
	}