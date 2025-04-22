cmake_minimum_required(VERSION 3.16)
project(StalcraftAuctionUI)

set(CMAKE_CXX_STANDARD 17)

# ImGui и ImPlot директории
set(IMGUI_DIR libs/imgui)
set(IMPLOT_DIR libs/implot)

# Основной исполняемый файл
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

# Подключение include директорий
target_include_directories(stalcraft_ui PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMPLOT_DIR}
    libs/json/single_include
)

# CPR как сабмодуль
set(CPR_ENABLE_SSL ON CACHE BOOL "Enable SSL")
add_subdirectory(libs/cpr)

# Найдём OpenGL и GLFW
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# JSON как header-only
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE libs/json/single_include)

# Линковка библиотек (все с ключевыми словами)
target_link_libraries(stalcraft_ui PRIVATE
    OpenGL::GL
    glfw
    cpr::cpr
    nlohmann_json
)

