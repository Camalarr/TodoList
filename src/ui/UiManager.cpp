#include "UiManager.hpp"

#include "renderer/Renderer.hpp"
#include "renderer/font_awesome.hpp"
#include "widgets/widgets.hpp"

void UiManager::render() {
	widgets::begin("##mainwindow", ImGui::GetMainViewport()->Size);

	ImVec2 buttons_pos = ImVec2(ImGui::GetWindowWidth() - 20, 10);
	if (widgets::icon_button("##Exit", ICON_FA_XMARK, buttons_pos))
		g_running = false;

	buttons_pos -= ImVec2(20, 0);
	if (::IsZoomed(g_renderer->m_hwnd)) {
		if (widgets::icon_button("##Restore", ICON_FA_WINDOW_RESTORE, buttons_pos))
			::ShowWindow(g_renderer->m_hwnd, SW_RESTORE);
	} else {
		if (widgets::icon_button("##Maximize", ICON_FA_WINDOW_MAXIMIZE, buttons_pos))
			::ShowWindow(g_renderer->m_hwnd, SW_MAXIMIZE);
	}

	buttons_pos -= ImVec2(20, 2);
	if (widgets::icon_button("##Minimize", ICON_FA_WINDOW_MINIMIZE, buttons_pos))
		::ShowWindow(g_renderer->m_hwnd, SW_SHOWMINNOACTIVE);

	draw_button_bar();

	ImGui::GetWindowDrawList()->AddLine(ImVec2(0, ImGui::GetCursorPosY()),
	                                    ImVec2(ImGui::GetWindowWidth(), ImGui::GetCursorPosY()),
	                                    Color(161, 209, 177).pack(),
	                                    2.f);

	widgets::childBegin(
	    "##Content",
	    ImVec2(0, ImGui::GetCursorPosY()),
	    ImVec2(ImGui::GetWindowWidth(), ImGui::GetContentRegionAvail().y));

	if (get_current_page() == eCurrentPage::HOME)
		draw_list_management();
	else
		draw_list_editor();

	widgets::childEnd();

	widgets::end();
}

void UiManager::draw_list_management() {
	const int item_width  = 300;
	const int min_spacing = 15;

	int max_lists_in_row = static_cast<int>((ImGui::GetContentRegionAvail().x + min_spacing) / (item_width + min_spacing));
	if (max_lists_in_row < 1)
		max_lists_in_row = 1;

	float space = 0.f;
	if (max_lists_in_row > 1)
		space = (ImGui::GetContentRegionAvail().x - (static_cast<float>(max_lists_in_row) * item_width)) / static_cast<float>(max_lists_in_row - 1);

	const auto& lists     = g_lists_manager->get_lists();
	int curr_lists_in_row = 1;

	if (!lists.empty()) {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(space, 10));
		for (size_t i = 0; i < lists.size(); ++i) {
			if (widgets::list(lists [ i ].get())) {
				g_lists_manager->set_selected_list(lists [ i ].get());

				set_current_page(eCurrentPage::LISTEDITOR);
			}

			if (curr_lists_in_row < max_lists_in_row) {
				ImGui::SameLine();
				curr_lists_in_row++;
			} else
				curr_lists_in_row = 1;
		}
		ImGui::PopStyleVar(1);
	}

	if (m_new_list_data.m_need_new_list) {
		if (!m_new_list_data.m_name_done && !m_new_list_data.m_desc_done) {
			if (widgets::input_popup("##NewListN", m_new_list_data.title_buffer, "Enter the list's name")) {
				if (!m_new_list_data.title_buffer.empty())
					m_new_list_data.m_name_done = true;
				else
					m_new_list_data.m_need_new_list = false;
			}
		}

		if (m_new_list_data.m_name_done) {
			if (widgets::input_popup("##NewListD", m_new_list_data.desc_buffer, "Enter the description (press enter to skip)"))
				m_new_list_data.m_desc_done = true;
		}


		if (m_new_list_data.m_name_done && m_new_list_data.m_desc_done) {
			g_lists_manager->add(std::make_unique<List>(m_new_list_data.title_buffer,
			                                            m_new_list_data.desc_buffer));

			m_new_list_data.m_need_new_list = false;
			m_new_list_data.m_name_done     = false;
			m_new_list_data.m_desc_done     = false;
			m_new_list_data.title_buffer    = "";
			m_new_list_data.desc_buffer     = "";
		}
	}
}

void UiManager::draw_list_editor() {
	List* list = g_lists_manager->get_selected_list();

	for (size_t i = 0; i < list->get_tasks().size(); ++i) {
		Task* task = list->get_task(i);

		widgets::task(task);
	}

	if (m_new_task_data.m_need_new_task) {
		if (widgets::input_popup("##NewTask", m_new_task_data.title_buffer, "Enter the task's name")) {
			if (!m_new_task_data.title_buffer.empty())
				m_new_task_data.m_name_done = true;
		}

		if (m_new_task_data.m_name_done) {
			list->add_task(std::make_unique<Task>(m_new_task_data.title_buffer));

			m_new_task_data.m_need_new_task = false;
			m_new_task_data.m_name_done     = false;
			m_new_task_data.title_buffer    = "";
		}
	}
}

void UiManager::draw_button_bar() {
	if (get_current_page() == eCurrentPage::HOME) {
		ImGui::SetCursorPos(ImVec2(10, 10));
		ImGui::SetWindowFontScale(2.f);
		ImGui::Text("Todo List");
		ImGui::SetWindowFontScale(1.f);

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 120, 40));
		if (widgets::button("Add List", ImVec2(100, 30)))
			m_new_list_data.m_need_new_list = true;

	} else if (g_lists_manager->get_selected_list()) {
		List* list = g_lists_manager->get_selected_list();
		ImGui::SetCursorPos(ImVec2(10, 10));
		ImGui::SetWindowFontScale(2.f);
		ImGui::Text(list->get_name().c_str());
		ImVec2 list_name_size = ImGui::CalcTextSize(list->get_name().c_str());
		ImGui::SetWindowFontScale(1.f);

		if (widgets::icon_button("##Home",
		                         ICON_FA_HOUSE,
		                         ImVec2(20 + list_name_size.x, 25),
		                         ImVec2(30, 30)))
			set_current_page(eCurrentPage::HOME);

		ImGui::PushFont(g_renderer->m_desc_font);
		widgets::wrapped_text(ImVec2(20, 12 + list_name_size.y),
		                      g_lists_manager->get_selected_list()->get_description(),
		                      Color(200, 200, 200),
		                      ImGui::GetWindowWidth() * 0.5f);
		ImGui::PopFont();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 120, 40));
		if (widgets::button("Add Task", ImVec2(100, 30)))
			m_new_task_data.m_need_new_task = true;
	}
}

eCurrentPage UiManager::get_current_page() const {
	return m_current_page;
}
void UiManager::set_current_page(eCurrentPage page) {
	m_current_page = page;
}