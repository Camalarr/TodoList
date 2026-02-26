#include "Folder.hpp"

#include "FileManager.hpp"

folder::folder(const std::filesystem::path& folder_path) :
    m_folder_path(folder_path) {
}

File folder::GetFile(std::filesystem::path file_name) const {
	if (file_name.is_absolute())
		throw std::invalid_argument("folder::GetFile requires a relative path.");

	if (file_name.string().contains(".."))
		throw std::invalid_argument("Relative path traversal is not allowed, "
		                            "refrain from using \"..\" in file paths.");

	return File(m_folder_path / file_name);
}

const std::filesystem::path folder::GetPath() const {
	return m_folder_path;
}