#include "File.hpp"

#include "FileManager.hpp"

File::File(const std::filesystem::path& file_path) : m_file_path(file_path) {
}

void File::operator=(const File& other) {
	m_file_path = other.m_file_path;
}

File::operator std::filesystem::path() {
	return m_file_path;
}

File::operator std::filesystem::path&() {
	return m_file_path;
}

bool File::Exists() const {
	return std::filesystem::exists(m_file_path);
}

const std::filesystem::path File::GetPath() const {
	return m_file_path;
}

File File::Move(std::filesystem::path new_path) {
	if (new_path.is_relative())
		new_path = m_file_path.parent_path() / new_path;

	FileManager::ensure_file_can_be_created(new_path);

	if (std::filesystem::exists(m_file_path))
		std::filesystem::rename(m_file_path, new_path);

	return {new_path};
}

void File::DeleteFile() {
	if (m_file_path.is_relative())
		m_file_path = m_file_path.parent_path() / m_file_path;

	if (std::filesystem::exists(m_file_path))
		std::filesystem::remove(m_file_path);
}