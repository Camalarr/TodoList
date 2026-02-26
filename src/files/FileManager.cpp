#include "FileManager.hpp"

void FileManager::Init(const std::filesystem::path& base_dir) {
	m_base_dir = base_dir;
	FileManager::ensure_folder_exists(m_base_dir);
}

const std::filesystem::path& FileManager::GetBaseDir() {
	return m_base_dir;
}

void FileManager::Rename(const std::filesystem::path& oldPath, const std::string& newName, const std::string& extension) {
	try {
		std::filesystem::path newPath = oldPath.parent_path() / newName;
		newPath.replace_extension(extension);

		if (!std::filesystem::exists(oldPath))
			return;

		if (std::filesystem::exists(newPath))
			return;

		std::filesystem::rename(oldPath, newPath);
	} catch (const std::exception& e) {
	}
}

File FileManager::GetProjectFile(std::filesystem::path file_path) {
	if (file_path.is_absolute())
		throw std::invalid_argument("Project files are relative to the BaseDir, don't use absolute paths!");

	if (file_path.string().contains(".."))
		throw std::invalid_argument("Relative path traversal is not allowed, "
		                            "refrain from using \"..\" in file paths.");

	return FileManager::ensure_file_can_be_created(m_base_dir / file_path);
}

folder FileManager::GetProjectFolder(std::filesystem::path folder_path) {
	if (folder_path.is_absolute())
		throw std::invalid_argument("Project folders are relative to the BaseDir, "
		                            "don't use absolute paths!");

	if (folder_path.string().contains(".."))
		throw std::invalid_argument("Relative path traversal is not allowed, "
		                            "refrain from using \"..\" in folder paths.");

	return FileManager::ensure_folder_exists(m_base_dir / folder_path);
}

std::filesystem::path FileManager::ensure_file_can_be_created(const std::filesystem::path file_path) {
	FileManager::ensure_folder_exists(file_path.parent_path());

	return file_path;
}

std::filesystem::path FileManager::ensure_folder_exists(const std::filesystem::path folder_path) {
	bool create_path = !std::filesystem::exists(folder_path);

	if (!create_path && !std::filesystem::is_directory(folder_path)) {
		std::filesystem::remove(folder_path);
		create_path = true;
	}

	if (create_path)
		std::filesystem::create_directory(folder_path);

	return folder_path;
}