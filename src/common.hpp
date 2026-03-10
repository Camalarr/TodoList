#pragma once

#include "files/FileManager.hpp"
#include "ui/settings.hpp"

#include <Windows.h>
#include <algorithm>
#include <any>
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <d3d11.h>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <random>
#include <sdkddkver.h>
#include <set>
#include <shellapi.h>
#include <shlobj.h>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>
#include <winsock2.h>

using namespace std::chrono_literals;

inline std::atomic_bool g_running = false;

inline tm* get_time() {
	time_t currentTime;
	struct tm* localTime;

	time(&currentTime);
	localTime = localtime(&currentTime);

	return localTime;
}

inline std::string GetDocumentsPath() {
	char path [ MAX_PATH ];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path)))
		return std::string(path);

	return "";
}