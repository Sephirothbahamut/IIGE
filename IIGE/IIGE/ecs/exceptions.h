#pragma once
/*
#include <stdexcept>
#include <initializer_list>
#include <string>

namespace iige::ecs::errors
	{
	namespace messages
		{
		std::string add_component(const std::string & component, const std::string & reason) noexcept
			{
			return "Error trying to add component \"" + component + "\".\n" + reason;
			}
		std::string missing_required_components(std::string component, std::initializer_list<std::string> components) noexcept
			{
			std::string ret{"The following components are required: "};
			for (const auto& component : components) { ret += component + ", "; }
			ret.pop_back(); ret.pop_back(); ret += ".";
			return ret;
			}
		}
	}*/