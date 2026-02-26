#pragma once

#include "Lists/ListsManager.hpp"

enum class eCurrentPage {
	HOME,
	LISTEDITOR
};

class UiManager {
public:
	UiManager()  = default;
	~UiManager() = default;

	void render();

	eCurrentPage get_current_page() const;
	void set_current_page(eCurrentPage page);

private:
	void draw_list_management();
	void draw_list_editor();
	void draw_button_bar();

private:
	struct new_list_data {
		bool m_need_new_list     = false;
		bool m_name_done         = false;
		bool m_desc_done         = false;
		std::string title_buffer = "";
		std::string desc_buffer  = "";
	} m_new_list_data;

	struct new_task_data {
		bool m_need_new_task     = false;
		bool m_name_done         = false;
		std::string title_buffer = "";
	} m_new_task_data;

	eCurrentPage m_current_page = eCurrentPage::HOME;
};

inline std::unique_ptr<UiManager> g_ui_manager = nullptr;