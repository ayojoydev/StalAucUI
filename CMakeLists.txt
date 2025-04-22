cmake_minimum_required(VERSION 3.16)
project(StalcraftAuctionUI)

set(CMAKE_CXX_STANDARD 17)

# Установи путь к ImGui и ImPlot
set(IMGUI_DIR libs/imgui)
set(IMPLOT_DIR libs/implot)

add_executable(stalcraft_ui
    main.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
)

# Include directories
target_include_directories(stalcraft_ui PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMPLOT_DIR}
    /usr/include
)

# Импорт зависимостей
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(CPR REQUIRED cpr)
pkg_check_modules(NLOHMANN_JSON REQUIRED nlohmann_json)

# Линковка
target_link_libraries(stalcraft_ui
    OpenGL::GL
    glfw
    cpr::cpr
    nlohmann_json::nlohmann_json
)
