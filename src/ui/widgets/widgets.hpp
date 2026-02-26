#pragma once

#include "ui/Lists/ListsManager.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace widgets {
	void begin(std::string_view name, ImVec2 size);
	void end();
	void childBegin(std::string_view name, ImVec2 pos, ImVec2 size);
	void childEnd();

	bool button(std::string_view label, const ImVec2& size_arg = ImVec2(0, 0));

	void text(const ImVec2& p_min, const ImVec2& p_max, Color col, std::string_view text, const ImVec2& align = ImVec2(0.f, 0.f));
	void wrapped_text(const ImVec2& pos, std::string_view label, Color color, float wrap_size, bool strikethrough = false);

	bool list(List* list);
	bool task(Task* task);

	bool begin_popup(const std::string_view id);
	void end_popup();

	bool icon_button(const std::string_view label, const char* icon, const ImVec2& pos, const ImVec2& size = ImVec2(15, 15));

	bool input_popup(const std::string_view id, std::string& buffer, const std::string_view name);
}