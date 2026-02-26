#pragma once
#include <filesystem>

class File;
class FileManager;

class folder {
public:
	folder(const std::filesystem::path& folder_path = "");

	File GetFile(std::filesystem::path file_name) const;
	const std::filesystem::path GetPath() const;

private:
	friend class FileManager;
	FileManager* m_file_manager;

	bool m_is_project_file;

	std::filesystem::path m_folder_path;
};