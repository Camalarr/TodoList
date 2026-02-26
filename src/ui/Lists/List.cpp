#include "List.hpp"

#include "ui/widgets/widgets.hpp"
#include "storage/Storage.hpp"

List::List(std::string name, std::string description) :
    m_name(name), m_description(description) {
	std::hash<std::string> hasher;
	m_id = hasher(name);

	tm time_info  = *get_time();
	m_creation_date = std::format("Created on {}/{}/{} at {}:{}:{}",
	                            time_info.tm_mday,
	                            time_info.tm_mon + 1,
	                            time_info.tm_year + 1900,
	                            time_info.tm_hour,
	                            time_info.tm_min,
	                            time_info.tm_sec);

	g_storage->save_list(this);
};

List::~List() {
	delete m_last_edited;
}

std::string List::get_name() const {
	return m_name;
}
std::string List::get_description() const {
	return m_description;
}
int List::get_id() const {
	return m_id;
}
std::string List::get_creation_date() const {
	return m_creation_date;
}
std::string List::get_last_edited() const {
	if (!m_last_edited)
		return "Never edited";

	std::string buffer = "Last edited : ";

	tm curr_time = *get_time();
	if (curr_time.tm_year - m_last_edited->tm_year != 0)
		buffer += std::format("{} years ago", curr_time.tm_year - m_last_edited->tm_year);
	else if (curr_time.tm_mon - m_last_edited->tm_mon != 0)
		buffer += std::format("{} months ago", curr_time.tm_mon - m_last_edited->tm_mon);
	else if (curr_time.tm_mday - m_last_edited->tm_mday != 0)
		buffer += std::format("{} days ago", curr_time.tm_mday - m_last_edited->tm_mday);
	else if (curr_time.tm_hour - m_last_edited->tm_hour != 0)
		buffer += std::format("{} hours ago", curr_time.tm_hour - m_last_edited->tm_hour);
	else if (curr_time.tm_min - m_last_edited->tm_min != 0)
		buffer += std::format("{} minutes ago", curr_time.tm_min - m_last_edited->tm_min);
	else
		buffer += "Now";


	return buffer;
}
const std::vector<std::unique_ptr<Task>>& List::get_tasks() const {
	return m_tasks;
}
int List::get_completed_tasks() const {
	int count = 0;
	for (const auto& task : m_tasks) {
		if (task->is_completed())
			count++;
	}
	return count;
}
Task* List::get_task(int index) {
	return m_tasks.at(index).get();
}

void List::update_last_edited() {
	if (!m_last_edited)
		m_last_edited = new tm();

	*m_last_edited = *get_time();

	g_storage->save_list(this);
}

void List::set_name(const std::string new_name) {
	m_name = new_name;

	std::hash<std::string> hasher;
	m_id = hasher(m_name);

	update_last_edited();
}
void List::set_description(const std::string new_description) {
	m_description = new_description;

	update_last_edited();
}

void List::add_task(std::unique_ptr<Task> task) {
	std::string original_name = task->get_name();
	int count                 = 1;
	bool exists               = true;

	while (exists) {
		exists                   = false;
		std::string current_name = original_name;
		if (count > 1)
			current_name = std::format("{} {}", original_name, count);

		for (const auto& t : m_tasks) {
			if (t->get_name() == current_name) {
				exists = true;
				break;
			}
		}

		if (exists)
			count++;
		else if (count > 1)
			task->set_name(current_name);
	}

	m_tasks.push_back(std::move(task));

	update_last_edited();
}
void List::remove_task(int index) {
	m_tasks.erase(m_tasks.begin() + index);

	update_last_edited();
}
void List::remove_task(const std::string& name) {
	std::erase_if(m_tasks, [ name ](const std::unique_ptr<Task>& task) {
		return task->get_name() == name;
	});

	update_last_edited();
}
void List::remove_all_tasks() {
	for (auto& task : m_tasks)
		task.reset();

	m_tasks.clear();

	update_last_edited();
}