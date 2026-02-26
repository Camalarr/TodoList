include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG        v3.12.0
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(nlohmann_json)