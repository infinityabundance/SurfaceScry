#include "window.hpp"

namespace surfacescry {

Window::Window(WindowInfo info, QObject *parent)
    : QObject(parent)
    , info_(std::move(info)) {
}

const WindowInfo &Window::info() const {
    return info_;
}

} // namespace surfacescry
