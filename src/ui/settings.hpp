#pragma once

#include <nlohmann/json.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>

struct Color {
	int r{255}, g{255}, b{255}, a{255};

	int pack() const {
		return a << 24 | b << 16 | g << 8 | r << 0;
	}

	ImVec4 to_vec4() const {
		return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	}

	Color operator+(const Color& other) const {
		Color result;
		result.r = std::clamp(r + other.r, 0, 255);
		result.g = std::clamp(g + other.g, 0, 255);
		result.b = std::clamp(b + other.b, 0, 255);
		return result;
	}

	Color operator-(const Color& other) const {
		Color result;
		result.r = std::clamp(r - other.r, 0, 255);
		result.g = std::clamp(g - other.g, 0, 255);
		result.b = std::clamp(b - other.b, 0, 255);
		return result;
	}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Color, r, g, b, a)
};

class settings {
public:
	Color main_color = Color(161, 209, 177, 255);
	Color grey_text = Color(220, 220, 220, 255);
	Color text_color = Color(255, 255, 255, 255);
	Color background_color = Color(25, 25, 25, 255);

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(settings, main_color, grey_text, text_color, background_color)
};

inline auto g_settings = settings();