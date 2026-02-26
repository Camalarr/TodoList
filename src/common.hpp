#pragma once

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

#define IMGUI_DEFINE_MATH_OPERATORS
#include "files/FileManager.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <imgui.h>

using namespace std::chrono_literals;

struct Color {
	int r{255}, g{255}, b{255}, a{255};

	int pack() {
		return a << 24 | b << 16 | g << 8 | r << 0;
	}

	ImVec4 to_vec4() const {
		return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	}
};

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