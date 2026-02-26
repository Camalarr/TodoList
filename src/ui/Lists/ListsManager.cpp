#include "ListsManager.hpp"

ListsManager::~ListsManager() {
	m_selected_list = nullptr;
	delete_all();
}

const std::vector<std::unique_ptr<List>>& ListsManager::get_lists() const {
	return m_lists;
}
List* ListsManager::get_by_id(int id) const {
	for (const auto& list : m_lists) {
		if (list->get_id() == id)
			return list.get();
	}
	return nullptr;
}
List* ListsManager::get_by_index(int index) const {
	if (index < 0 || index >= m_lists.size())
		return nullptr;

	return m_lists.at(index).get();
}
List* ListsManager::get_selected_list() const {
	return m_selected_list;
}

void ListsManager::set_selected_list(List* list) {
	m_selected_list = list;
}

void ListsManager::add(std::unique_ptr<List> list) {
	std::string original_name = list->get_name();
	int count                 = 1;
	bool exists               = true;

	while (exists) {
		exists                   = false;
		std::string current_name = original_name;
		if (count > 1)
			current_name = std::format("{} {}", original_name, count);

		for (const auto& l : m_lists) {
			if (l->get_name() == current_name) {
				exists = true;
				break;
			}
		}

		if (exists)
			count++;
		else if (count > 1)
			list->set_name(current_name);
	}

	m_lists.push_back(std::move(list));
}
void ListsManager::delete_by_id(int id) {
	std::erase_if(m_lists, [ id ](const std::unique_ptr<List>& list) {
		return list->get_id() == id;
	});
}
void ListsManager::delete_by_index(int index) {
	if (index < 0 || index >= m_lists.size())
		return;

	m_lists.erase(m_lists.begin() + index);
}
void ListsManager::delete_all() {
	for (auto& list : m_lists)
		list.reset();

	m_lists.clear();
}