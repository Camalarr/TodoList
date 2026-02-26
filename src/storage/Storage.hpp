#pragma once

#include <nlohmann/json.hpp>

class List;

class Storage {
public:
    Storage();
    ~Storage();

    void save_lists();
    void save_list(List* list);
    void load_lists();
    void load_list(const std::string& file_name);
};

inline std::unique_ptr<Storage> g_storage = nullptr;