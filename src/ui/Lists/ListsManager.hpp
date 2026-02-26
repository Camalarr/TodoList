#pragma once

#include "List.hpp"

class ListsManager {
public:
	ListsManager() = default;
	~ListsManager();

	const std::vector<std::unique_ptr<List>>& get_lists() const;
	List* get_by_id(int id) const;
	List* get_by_index(int index) const;
	List* get_selected_list() const;

	void set_selected_list(List* list);

	void add(std::unique_ptr<List> list);

	void delete_by_id(int id);
	void delete_by_index(int index);
	void delete_all();

private:
	std::vector<std::unique_ptr<List>> m_lists;
	List* m_selected_list = nullptr;
};

inline std::unique_ptr<ListsManager> g_lists_manager;