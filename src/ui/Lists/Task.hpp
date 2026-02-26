#pragma once

class Task {
public:
	Task(const std::string& name) : m_name(name), m_completed(false) {
	}

	const std::string& get_name() const {
		return m_name;
	}
	bool is_completed() const {
		return m_completed;
	}

	void set_name(const std::string& name) {
		m_name = name;
	}
	void set_value(bool value) {
		m_completed = value;
	}

public:
	bool m_need_rename = false;

private:
	std::string m_name;
	bool m_completed = false;
};