#pragma once

#include <utils/MS/window/window.h>
#include <utils/MS/graphics/d2d.h>
#include <utils/MS/graphics/d2d/window.h>

namespace iige
	{
	class window : public utils::MS::window::base
		{
		public:
			using render_module_type = utils::MS::graphics::d2d::window::composition_swap_chain;

			struct create_info
				{
				utils::MS::window::base::create_info base;
				utils::MS::graphics::initializer graphics;
				};

			window(const create_info& create_info) : utils::MS::window::base
				{
				create_info.base,
				render_module_type::create_info
					{
					.d2d_device{create_info.graphics}
					}
				},
				render_module_ptr{utils::MS::window::base::get_module_ptr<render_module_type>()}
				{
				}

			utils::observer_ptr<render_module_type> render_module_ptr{nullptr};
		};
	}