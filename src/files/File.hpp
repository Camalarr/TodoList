#pragma once
#include <filesystem>

class FileManager;

class File {
public:
	File(const std::filesystem::path& file_path = "");
	void operator=(const File& other);
	operator std::filesystem::path();
	operator std::filesystem::path&();

	bool Exists() const;
	const std::filesystem::path GetPath() const;
	File Move(std::filesystem::path new_path);
	void DeleteFile();

private:
	friend class FileManager;

	bool m_is_project_file;
	std::filesystem::path m_file_path;
};