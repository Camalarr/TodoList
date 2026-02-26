#include "renderer/Renderer.hpp"
#include "ui/Lists/ListsManager.hpp"
#include "ui/UiManager.hpp"
#include "storage/Storage.hpp"

#include <chrono>
#include <shlobj.h>

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	try {
		std::filesystem::path doc_path = GetDocumentsPath();
		doc_path /= "TodoApp";
		g_file_manager.Init(doc_path);
		g_file_manager.GetProjectFolder("Data");

		g_renderer      = std::make_unique<Renderer>();
		g_ui_manager    = std::make_unique<UiManager>();
		g_lists_manager = std::make_unique<ListsManager>();
		g_storage       = std::make_unique<Storage>();

		g_renderer->add_dx_callback(
		    [] {
			    g_ui_manager->render();
		    },
		    0);

		g_running = true;
		while (g_running) {
			g_renderer->OnPresent();
			std::this_thread::sleep_for(1ms);
		}

		g_storage.reset();
		g_lists_manager.reset();
		g_ui_manager.reset();
		g_renderer.reset();
	} catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR | MB_OK);
	}

	return 0;
}