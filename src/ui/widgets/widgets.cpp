#include "widgets.hpp"

#include "renderer/Renderer.hpp"
#include "renderer/font_awesome.hpp"
#include "ui/UiManager.hpp"

#include <misc/cpp/imgui_stdlib.h>

namespace widgets {
	const DWORD window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

	void begin(std::string_view name, ImVec2 size) {
		ImGui::Begin(name.data(), nullptr, window_flags);
		ImGui::SetWindowSize(size);

		ImGui::GetCurrentWindow()->DrawList->AddRectFilled({0, 0},
		                                                   ImVec2(size.x - 1, size.y - 1),
		                                                   Color(25, 25, 25).pack(),
		                                                   10.f);

		// style
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding              = 10.f;
		style.Alpha                       = 1;
		style.WindowBorderSize            = 0.f;
		style.ChildRounding               = 10.f;
		style.FrameBorderSize             = 0.f;
		style.FrameRounding               = 10.f;
		style.WindowPadding               = ImVec2(10, 10);
		style.Colors [ ImGuiCol_ChildBg ] = Color(0, 0, 0, 0).to_vec4();
		style.Colors [ ImGuiCol_TextSelectedBg ] = Color(161, 209, 177).to_vec4();
		style.Colors [ ImGuiCol_ResizeGrip ] = Color(0, 0, 0, 0).to_vec4();
		style.Colors [ ImGuiCol_ResizeGripHovered ] = Color(0, 0, 0, 0).to_vec4();
		style.Colors [ ImGuiCol_ResizeGripActive ] = Color(0, 0, 0, 0).to_vec4();
		style.Colors [ ImGuiCol_Separator ] = Color(0, 0, 0, 0).to_vec4();
		style.Colors [ ImGuiCol_SeparatorHovered ] = Color(0, 0, 0, 0).to_vec4();
		style.Colors [ ImGuiCol_SeparatorActive ] = Color(0, 0, 0, 0).to_vec4();
		style.ScrollbarSize                       = 5.f;
		style.ScrollbarRounding                   = 10;
		style.ItemSpacing                         = ImVec2(10, 15);

		// move window
		GetWindowRect(g_renderer->m_hwnd, &g_renderer->rc);
		MoveWindow(g_renderer->m_hwnd,
		           g_renderer->rc.left + ImGui::GetWindowPos().x,
		           g_renderer->rc.top + ImGui::GetWindowPos().y,
		           1000,
		           700,
		           TRUE);
		ImGui::SetWindowPos(ImVec2(0.f, 0.f));
	}

	void end() {
		ImGui::End();
	}

	void childBegin(std::string_view name, ImVec2 pos, ImVec2 size) {
		ImGui::SetCursorPos(pos);
		ImGui::BeginChild(name.data(), size, ImGuiChildFlags_AlwaysUseWindowPadding);
	}

	void childEnd() {
		ImGui::EndChild();
	}

	bool task(Task* task) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		List* list          = g_lists_manager->get_selected_list();


		if (window->SkipItems)
			return false;

		ImGui::SetWindowFontScale(1.5f);
		const ImGuiID id       = window->GetID(task->get_name().c_str());
		const float wrap_width = ImGui::GetContentRegionAvail().x - 50.f;
		const ImVec2 label_size = ImGui::CalcTextSize(task->get_name().c_str(), NULL, false, wrap_width);
		const ImVec2 pos = window->DC.CursorPos;
		const ImRect rect(
		    pos,
		    pos + ImVec2(ImGui::GetContentRegionAvail().x, label_size.y + 10.f));

		ImGui::ItemSize(rect, 0.f);

		if (!ImGui::ItemAdd(rect, id))
			return false;

		bool hovered, held, pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held);

		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 10.f);
		ImGui::PushStyleColor(ImGuiCol_Border, Color(161, 209, 177).pack());
		if (begin_popup(task->get_name().c_str())) {
			if (ImGui::GetCurrentWindow()->BeginCount == 1) {
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 10));
				if (button("Delete")) {
					list->remove_task(task->get_name());
					ImGui::PopStyleVar(1);
					end_popup();
					return false;
				}

				ImGui::SameLine();
				if (button("Rename"))
					task->m_need_rename = true;

				ImGui::PopStyleVar(1);
			}
			end_popup();
		}
		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);

		Color background = Color(68, 68, 68, 255);

		if (task->is_completed() || pressed || held)
			background = Color(161, 209, 177, 255);
		else if (hovered)
			background = Color(100, 100, 100, 255);

		if (ImGui::IsItemClicked()) {
			task->set_value(!task->is_completed());
			list->update_last_edited();
			ImGui::MarkItemEdited(id);
		}

		ImGui::GetWindowDrawList()->AddRectFilled(rect.Min,
		                                          rect.Min + ImVec2(30, 30),
		                                          background.pack(),
		                                          5.f);

		wrapped_text(rect.Min + ImVec2(50, 0), task->get_name(), Color(255, 255, 255), wrap_width, task->is_completed());
		ImGui::SetWindowFontScale(1.f);

		if (task->is_completed()) {
			ImGui::RenderCheckMark(ImGui::GetWindowDrawList(),
			                       rect.Min + ImVec2(5.f, 5.f),
			                       Color(0, 0, 0).pack(),
			                       20.f);
		}

		if (task->m_need_rename) {
			if (ImGui::IsPopupOpen(task->get_name().c_str()))
				ImGui::CloseCurrentPopup();

			std::string buffer;

			if (widgets::input_popup("Rename List", buffer, "Enter the new name for the task")) {
				if (!buffer.empty()) {
					task->set_name(buffer);
					list->update_last_edited();
				}
				task->m_need_rename = false;
			}
		}

		return pressed;
	}

	bool button(std::string_view label, const ImVec2& size_arg) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		const ImGuiID id        = window->GetID(label.data());
		const ImVec2 label_size = ImGui::CalcTextSize(label.data());
		const ImVec2 pos        = window->DC.CursorPos;
		const ImVec2 size =
		    ImGui::CalcItemSize(size_arg, label_size.x, label_size.y);
		const ImRect bb(pos, pos + size);

		ImGui::ItemSize(size);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held, pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		Color bg_color = Color(125, 125, 125, 0);

		if (held || pressed)
			bg_color = Color(196, 236, 209, 50);
		else if (hovered)
			bg_color = Color(130, 110, 200, 50);

		window->DrawList->AddRectFilled(bb.Min - ImVec2(5, 5),
		                                bb.Max + ImVec2(5, 5),
		                                bg_color.pack(),
		                                10.f);
		window->DrawList->AddRect(bb.Min - ImVec2(5, 5),
		                          bb.Max + ImVec2(5, 5),
		                          Color(161, 209, 177).pack(),
		                          10.f);

		text(bb.Min - ImVec2(5, 5), bb.Max + ImVec2(5, 5), Color(255, 255, 255), label, ImVec2(0.5f, 0.5f));

		return pressed;
	}

	void text(const ImVec2& pos_min, const ImVec2& pos_max, Color col, std::string_view text, const ImVec2& align) {
		ImGui::PushStyleColor(ImGuiCol_Text, col.pack());
		ImGui::RenderTextClipped(pos_min, pos_max, text.data(), NULL, NULL, align, NULL);
		ImGui::PopStyleColor(1);
	}

	void wrapped_text(const ImVec2& pos, std::string_view label, Color color, float wrap_size, bool strikethrough) {
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImFont* font          = ImGui::GetFont();

		const char* text_end = label.data() + label.size();
		const char* s        = label.data();
		ImVec2 cursor        = pos;

		while (s < text_end) {
			const char* word_wrap_end = text_end;
			if (wrap_size > 0.f) {
				float scale = ImGui::GetCurrentWindow()->FontWindowScale * font->Scale;
				word_wrap_end = font->CalcWordWrapPositionA(scale, s, text_end, wrap_size);
			}


			if (word_wrap_end == s && s < text_end)
				word_wrap_end++;

			float font_size = ImGui::GetFontSize();
			draw_list->AddText(font, font_size, cursor, color.pack(), s, word_wrap_end);

			if (strikethrough) {
				ImVec2 line_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.f, s, word_wrap_end);
				float mid_y = cursor.y + line_size.y * 0.6f;
				draw_list->AddLine(ImVec2(cursor.x, mid_y),
				                   ImVec2(cursor.x + line_size.x, mid_y),
				                   color.pack(),
				                   2.f);
			}

			cursor.y += font_size;
			s = word_wrap_end;

			if (s < text_end && *s == '\n')
				s++;
			else {
				while (s < text_end && *s == ' ')
					s++;
			}
		}
	}

	bool list(List* list) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems)
			return false;

		const ImGuiID id  = window->GetID(list->get_name().c_str());
		const ImVec2 pos  = window->DC.CursorPos;
		const ImVec2 size = ImVec2(300, 150);
		const ImRect bb(pos, pos + size);

		ImGui::ItemSize(bb.GetSize(), ImGui::GetStyle().FramePadding.y);

		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held, pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 10.f);
		ImGui::PushStyleColor(ImGuiCol_Border, Color(161, 209, 177).pack());

		if (begin_popup(list->get_name().c_str())) {
			if (ImGui::GetCurrentWindow()->BeginCount == 1) {
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 10));
				if (button("Delete")) {
					g_file_manager
					    .GetProjectFile("Data/" + list->get_name() + ".json")
					    .DeleteFileA();
					g_lists_manager->delete_by_id(list->get_id());

					ImGui::PopStyleVar(2);
					ImGui::PopStyleColor(1);
					end_popup();
					return false;
				}

				ImGui::SameLine();
				if (button("Rename"))
					list->m_need_rename = true;

				ImGui::SameLine();
				if (button("Change description"))
					list->m_need_desc_rename = true;

				ImGui::PopStyleVar(1);

				const int tasks_size = static_cast<int>(list->get_tasks().size());
				const std::string task_str = std::format("Tasks : {}/{} - {}%", list->get_completed_tasks(), tasks_size, tasks_size > 0 ? (list->get_completed_tasks() * 100 / tasks_size) : 0);

				ImGui::PushTextWrapPos(300.f);
				ImGui::PushStyleColor(ImGuiCol_Text, Color(220, 220, 220).pack());

				ImGui::TextWrapped("%s", list->get_creation_date().c_str());
				ImGui::TextWrapped("%s", list->get_last_edited().c_str());
				ImGui::TextWrapped("%s", task_str.c_str());

				ImGui::PopStyleColor(1);
				ImGui::PopTextWrapPos();
			}
			end_popup();
		}

		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);

		Color bg_color = Color(125, 125, 125, 50);

		if (held || pressed)
			bg_color = Color(196, 236, 209, 50);
		else if (hovered)
			bg_color = Color(130, 110, 200, 50);

		ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, bg_color.pack(), 10.f);

		ImGui::GetWindowDrawList()->AddRect(bb.Min, bb.Max, Color(161, 209, 177).pack(), 10.f);

		const ImVec2 name_size = ImGui::CalcTextSize(list->get_name().c_str(), nullptr, false, bb.GetWidth() - 20.f);

		ImGui::SetWindowFontScale(1.2f);
		wrapped_text(bb.Min + ImVec2(10, 5), list->get_name(), Color(255, 255, 255), bb.GetWidth() - 30.f);
		ImGui::SetWindowFontScale(1.f);

		const ImVec2 description_pos = bb.Min + ImVec2(15, name_size.y + 10.f);

		ImGui::PushFont(g_renderer->m_desc_font);
		wrapped_text(description_pos, list->get_description(), Color(220, 220, 220), bb.GetWidth() - 30.f);
		ImGui::PopFont();

		if (list->m_need_rename) {
			if (ImGui::IsPopupOpen(list->get_name().c_str()))
				ImGui::CloseCurrentPopup();

			std::string buffer;

			if (widgets::input_popup("Rename List", buffer, "Enter the new name for the list")) {
				if (!buffer.empty())
					list->set_name(buffer);
				list->m_need_rename = false;
			}
		} else if (list->m_need_desc_rename) {
			if (ImGui::IsPopupOpen(list->get_name().c_str()))
				ImGui::CloseCurrentPopup();

			std::string buffer;

			if (widgets::input_popup("New List desc", buffer, "Enter the new list's description")) {
				if (!buffer.empty())
					list->set_description(buffer);
				list->m_need_desc_rename = false;
			}
		}

		return pressed;
	}

	bool begin_popup(const std::string_view id) {
		if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked [ 1 ])
			ImGui::OpenPopup(id.data());

		if (ImGui::BeginPopup(id.data(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
			return true;

		return false;
	}

	void end_popup() {
		ImGui::EndPopup();
	}

	bool icon_button(const std::string_view label, const char* icon, const ImVec2& pos, const ImVec2& size) {
		ImGui::SetCursorPos(pos);
		const bool pressed = ImGui::InvisibleButton(label.data(), size);

		Color icon_color = Color(255, 255, 255);
		if (ImGui::IsItemHovered())
			icon_color = Color(161, 209, 177);

		const ImVec2 min       = ImGui::GetItemRectMin();
		const ImVec2 text_size = ImGui::CalcTextSize(icon);
		const ImVec2 text_pos  = {min.x + (15 - text_size.x) / 2,
		                          min.y + (15 - text_size.y) / 2};

		ImGui::PushFont(g_renderer->m_font_awesome);
		ImGui::GetWindowDrawList()->AddText(text_pos, icon_color.pack(), icon);
		ImGui::PopFont();

		return pressed;
	}

	bool input_popup(const std::string_view id, std::string& buffer, const std::string_view name) {
		const ImVec2 window_size = ImGui::FindWindowByName("##mainwindow")->Size;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(window_size);
		ImGui::Begin(id.data(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);

		ImDrawList* dl        = ImGui::GetWindowDrawList();
		const ImVec2 box_size = ImVec2(window_size.x / 2, 70);
		const ImVec2 box_pos  = (window_size / 2) - (box_size / 2);

		dl->AddRectFilled(ImVec2(0, 0), window_size, Color(100, 100, 100, 150).pack());

		dl->AddRectFilled(box_pos, box_pos + box_size, Color(20, 20, 20).pack(), 10.f);

		ImGui::SetCursorPos(box_pos + ImVec2(10, 5));
		ImGui::SetWindowFontScale(1.25f);
		ImGui::Text(name.data());

		ImGui::SetCursorPos(box_pos + ImVec2(10, 35));
		ImGui::PushItemWidth(box_size.x - 20.f);
		ImGui::PushFont(g_renderer->m_desc_font);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, Color(45, 45, 45).to_vec4());
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Color(0, 0, 0, 0).to_vec4());

		ImGui::SetKeyboardFocusHere();

		const bool enter_pressed = ImGui::InputText("##inputtext", &buffer, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AlwaysOverwrite | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_WordWrap);

		ImGui::PopStyleColor(2);
		ImGui::PopFont();
		ImGui::PopItemWidth();
		ImGui::SetWindowFontScale(1.f);

		ImGui::End();
		return enter_pressed;
	}
}