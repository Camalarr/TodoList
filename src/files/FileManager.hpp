#pragma once

#include "File.hpp"
#include "Folder.hpp"

class FileManager final {
public:
	FileManager()                                  = default;
	virtual ~FileManager()                         = default;
	FileManager(const FileManager&)                = delete;
	FileManager(FileManager&&) noexcept            = delete;
	FileManager& operator=(const FileManager&)     = delete;
	FileManager& operator=(FileManager&&) noexcept = delete;

	void Init(const std::filesystem::path& base_dir);

	const std::filesystem::path& GetBaseDir();

	File GetProjectFile(std::filesystem::path file_path);
	folder GetProjectFolder(std::filesystem::path folder_path);

	void Rename(const std::filesystem::path& oldPath, const std::string& newName, const std::string& extension = ".txt");

	static std::filesystem::path ensure_file_can_be_created(const std::filesystem::path file_path);
	static std::filesystem::path ensure_folder_exists(const std::filesystem::path folder_path);

private:
	std::filesystem::path m_base_dir;
};

inline auto g_file_manager = FileManager();