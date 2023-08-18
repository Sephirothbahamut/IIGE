#pragma once

#include <vector>
#include <functional>
#include <chrono>

#include <utils/clock.h>
#include <utils/memory.h>
#include <utils/logger.h>

#include "window.h"

namespace iige
	{
	namespace loop
		{
		struct actions_interface
			{
			virtual bool is_running() const noexcept = 0;
			virtual void step(float delta_time) = 0;
			virtual void draw(float delta_time, float interpolation) = 0;
			};

		namespace details
			{
			using clock = utils::clock<std::chrono::steady_clock, float>; //float seconds

			template <std::derived_from<actions_interface> loop_actions_t> 
			struct base_loop 
				{
				base_loop(loop_actions_t& loop_actions) : loop_actions{loop_actions} {}

				virtual void run() = 0; 

				bool is_running() const noexcept                 { return loop_actions.get().is_running(); }
				void step(float delta_time                     ) {        loop_actions.get().step(delta_time               ); }
				void draw(float delta_time, float interpolation) {        loop_actions.get().draw(delta_time, interpolation); }

				std::reference_wrapper<loop_actions_t> loop_actions;
				};
			}

		template <std::derived_from<actions_interface> loop_actions_t>
		class fixed_game_speed_variable_framerate : public details::base_loop<loop_actions_t>
			{
			private:
				const float steps_per_second = 1.f;
				const size_t max_frameskip = 5;

				details::clock::duration_t step_delta_time;

			public:
				fixed_game_speed_variable_framerate(loop_actions_t& loop_actions, float steps_per_second = 1.f, size_t max_frameskip = 5) noexcept :
					details::base_loop<loop_actions_t>{loop_actions},
					steps_per_second{steps_per_second}, max_frameskip{max_frameskip}, step_delta_time{std::chrono::seconds{1} / steps_per_second}
					{}

				using details::base_loop<loop_actions_t>::is_running;
				using details::base_loop<loop_actions_t>::step;
				using details::base_loop<loop_actions_t>::draw;

				void run()
					{
					// https://dewitters.com/dewitters-gameloop/
					details::clock clock;
					details::clock::duration_t next_step_time{clock.get_elapsed()};
					size_t step_loops{0};
					float interpolation{0};

					details::clock fps_clock;
					uint32_t frames_counter{0};

					while (is_running())
						{
						if (fps_clock.get_elapsed() > std::chrono::seconds{1})
							{
							utils::globals::logger.log("FPS: " + std::to_string(frames_counter / fps_clock.restart().count()));
							frames_counter = 0;
							}
						while (clock.get_elapsed() > next_step_time && step_loops < max_frameskip)
							{
							step(step_delta_time.count());
							step_loops++;
							next_step_time += step_delta_time;
							}
						step_loops = 0;

						interpolation = (clock.get_elapsed() + step_delta_time - next_step_time) / step_delta_time;

						frames_counter++;
						draw(step_delta_time.count(), interpolation);
						}
					}
			};

		template <std::derived_from<actions_interface> loop_actions_t>
		class fixed_fps_and_game_speed : public details::base_loop<loop_actions_t>
			{
			private:
				const float steps_per_second = 1.f;

				details::clock::duration_t step_delta_time;

			public:
				fixed_fps_and_game_speed(loop_actions_t& loop_actions, float steps_per_second = 1.f) noexcept :
					details::base_loop<loop_actions_t>{loop_actions},
					steps_per_second{steps_per_second}, step_delta_time{std::chrono::seconds{1} / steps_per_second}
					{}

				using details::base_loop<loop_actions_t>::is_running;
				using details::base_loop<loop_actions_t>::step;
				using details::base_loop<loop_actions_t>::draw;

				void run()
					{
					// https://dewitters.com/dewitters-gameloop/

					details::clock clock;

					details::clock::duration_t next_step_time = clock.get_elapsed();

					details::clock::duration_t sleep_time{std::chrono::milliseconds{0}};

					details::clock fps_clock;
					uint32_t frames_counter{0};

					while (is_running())
						{
						if (fps_clock.get_elapsed() > std::chrono::milliseconds{1})
							{
							utils::globals::logger.log("FPS: " + std::to_string(frames_counter / fps_clock.restart().count()));
							frames_counter = 0;
							}
						
						step(step_delta_time.count());

						frames_counter++;
						draw(step_delta_time.count(), 0);

						next_step_time += step_delta_time;
						sleep_time = next_step_time - clock.get_elapsed();
						if (sleep_time >= std::chrono::milliseconds{0}) { std::this_thread::sleep_for(sleep_time); }
						}
					}
			};

		template <std::derived_from<actions_interface> loop_actions_t>
		class variable_fps_and_game_speed : public details::base_loop<loop_actions_t>
			{
			private:

			public:
				variable_fps_and_game_speed(loop_actions_t& loop_actions) noexcept : details::base_loop<loop_actions_t>{loop_actions} {}

				using details::base_loop<loop_actions_t>::is_running;
				using details::base_loop<loop_actions_t>::step;
				using details::base_loop<loop_actions_t>::draw;

				void run()
					{
					// https://dewitters.com/dewitters-gameloop/
					details::clock clock;

					details::clock::duration_t prev_step_time;
					details::clock::duration_t curr_step_time{clock.get_elapsed()};
					
					details::clock::duration_t step_delta_time;

					details::clock fps_clock;
					uint32_t frames_counter{0};

					while (is_running())
						{
						if (fps_clock.get_elapsed() > std::chrono::seconds{1})
							{
							utils::globals::logger.log("FPS: " + std::to_string(frames_counter / fps_clock.restart().count()));
							frames_counter = 0;
							}
						
						prev_step_time = curr_step_time;
						curr_step_time = clock.get_elapsed();

						step_delta_time = curr_step_time - prev_step_time;

						step(step_delta_time.count());
						frames_counter++;
						draw(step_delta_time.count(), 0);
						}
					}

			};
		}
	}