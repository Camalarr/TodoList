#pragma once

#include "Task.hpp"

class List {
	friend class Storage;
public:
	List(std::string name, std::string description);
	~List();

	std::string get_name() const;
	std::string get_description() const;
	int get_id() const;
	std::string get_creation_date() const;
	std::string get_last_edited() const;

	const std::vector<std::unique_ptr<Task>>& get_tasks() const;
	int get_completed_tasks() const;
	Task* get_task(int index);

	void set_name(const std::string new_name);
	void set_description(const std::string new_description);

	void add_task(std::unique_ptr<Task> task);
	void remove_task(int index);
	void remove_task(const std::string& name);
	void remove_all_tasks();

	void update_last_edited();

public:
	bool m_need_rename      = false;
	bool m_need_desc_rename = false;

private:
	std::string m_name        = "";
	std::string m_description = "";
	int m_id                  = 0;
	std::string m_creation_date = "";
	tm* m_last_edited           = nullptr;

	std::vector<std::unique_ptr<Task>> m_tasks;
};