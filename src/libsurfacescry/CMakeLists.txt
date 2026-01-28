add_library(surfacescry)

target_sources(surfacescry
    PRIVATE
        window_types.cpp
        window.cpp
        window_manager.cpp
        kde_window_manager_backend.cpp
    PUBLIC
        window_types.hpp
        window.hpp
        window_manager.hpp
        kde_window_manager_backend.hpp
)

target_include_directories(surfacescry
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(surfacescry
    PUBLIC
        Qt6::Core
        KF6::WindowSystem
        KF6::CoreAddons
)

set_target_properties(surfacescry PROPERTIES
    AUTOMOC ON
)
