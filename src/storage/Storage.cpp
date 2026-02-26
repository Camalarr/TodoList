#include "Storage.hpp"

#include "ui/Lists/List.hpp"
#include "ui/Lists/ListsManager.hpp"

Storage::Storage() {
	load_lists();
}

Storage::~Storage() {
	save_lists();
}

void Storage::save_lists() {
	for (auto& list : g_lists_manager->get_lists()) {
		save_list(list.get());
	}
}

void Storage::save_list(List* list) {
	nlohmann::json json;

	json [ "Name" ]         = list->get_name();
	json [ "Description" ]  = list->get_description();
	json [ "ID" ]           = list->get_id();
	json [ "CreationDate" ] = list->get_creation_date();

	if (list->m_last_edited) {
		json [ "LastEditedTime" ][ "Year" ] = list->m_last_edited->tm_year + 1900;
		json [ "LastEditedTime" ][ "Month" ]  = list->m_last_edited->tm_mon;
		json [ "LastEditedTime" ][ "Day" ]    = list->m_last_edited->tm_mday;
		json [ "LastEditedTime" ][ "Hour" ]   = list->m_last_edited->tm_hour;
		json [ "LastEditedTime" ][ "Minute" ] = list->m_last_edited->tm_min;
		json [ "LastEditedTime" ][ "Second" ] = list->m_last_edited->tm_sec;
	} else {
		json [ "LastEditedTime" ] = nullptr;
	}

	json [ "Tasks" ] = nlohmann::json::array();
	for (size_t i = 0; i < list->get_tasks().size(); ++i) {
		json [ "Tasks" ].push_back({{"Name", list->get_task(i)->get_name()},
		                            {"Completed", list->get_task(i)->is_completed()}});
	}

	const std::filesystem::path file =
	    g_file_manager.GetProjectFile("Data/" + list->get_name() + ".json").GetPath();

	std::ofstream(file) << json.dump(4);
}

void Storage::load_lists() {
	for (const auto& file : std::filesystem::directory_iterator(
	         g_file_manager.GetProjectFolder("Data").GetPath())) {
		if (file.is_regular_file()) {
			if (file.path().extension() == ".json")
				load_list(file.path().filename().generic_string());
		}
	}
}

void Storage::load_list(const std::string& file_name) {
	const std::filesystem::path file =
	    g_file_manager.GetProjectFile("Data/" + file_name).GetPath();

	if (!std::filesystem::exists(file))
		return;

	std::ifstream f(file);
	if (!f || f.peek() == std::ifstream::traits_type::eof())
		return;

	nlohmann::json json;
	f >> json;

	std::unique_ptr<List> list = std::make_unique<List>(json [ "Name" ].get<std::string>(),
	                                                    json [ "Description" ].get<std::string>());

	list->m_id            = json [ "ID" ].get<int>();
	list->m_creation_date = json [ "CreationDate" ].get<std::string>();

	for (const auto& task : json [ "Tasks" ]) {
		std::unique_ptr<Task> new_task = std::make_unique<Task>(task [ "Name" ].get<std::string>());
		new_task->set_value(task [ "Completed" ].get<bool>());
		list->add_task(std::move(new_task));
	}

	if (!json [ "LastEditedTime" ].is_null()) {
		list->m_last_edited = new tm();
		list->m_last_edited->tm_year = json [ "LastEditedTime" ][ "Year" ].get<int>() - 1900;
		list->m_last_edited->tm_mon = json [ "LastEditedTime" ][ "Month" ].get<int>();
		list->m_last_edited->tm_mday = json [ "LastEditedTime" ][ "Day" ].get<int>();
		list->m_last_edited->tm_hour = json [ "LastEditedTime" ][ "Hour" ].get<int>();
		list->m_last_edited->tm_min = json [ "LastEditedTime" ][ "Minute" ].get<int>();
		list->m_last_edited->tm_sec = json [ "LastEditedTime" ][ "Second" ].get<int>();
	}

	g_lists_manager->add(std::move(list));
}