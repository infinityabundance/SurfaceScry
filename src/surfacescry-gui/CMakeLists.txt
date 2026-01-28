qt_add_resources(SURFACESCRY_GUI_RESOURCES ${PROJECT_SOURCE_DIR}/resources/surfacescry.qrc)

add_executable(surfacescry-gui
    main.cpp
    MainWindow.cpp
    WindowTreeModel.cpp
    PropertyInspectorWidget.cpp
    EventLogModel.cpp
    EventLogView.cpp
    ${SURFACESCRY_GUI_RESOURCES}
)

target_include_directories(surfacescry-gui
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/libsurfacescry
)

target_link_libraries(surfacescry-gui
    PRIVATE
        surfacescry
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        KF6::WindowSystem
        KF6::CoreAddons
)

set_target_properties(surfacescry-gui PROPERTIES
    AUTOMOC ON
)
