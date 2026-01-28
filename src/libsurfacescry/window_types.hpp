#pragma once

#include <string>
#include <vector>

#include <sys/types.h>

namespace surfacescry {

enum class WindowBackend { X11, Wayland, Unknown };

enum class WindowState { Active, Minimized, Maximized, Fullscreen, Hidden, Normal };

struct WindowGeometry {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

struct WindowInfo {
    std::string id;
    std::string title;
    std::string appName;
    std::string appId;
    pid_t pid = 0;
    WindowBackend backend = WindowBackend::Unknown;
    WindowGeometry geometry;
    std::vector<WindowState> states;
    int desktopIndex = -1;
    int zOrder = 0;
};

} // namespace surfacescry
