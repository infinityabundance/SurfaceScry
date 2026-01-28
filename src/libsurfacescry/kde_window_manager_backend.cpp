#include "kde_window_manager_backend.hpp"

namespace surfacescry {

KDEWindowManagerBackend::KDEWindowManagerBackend(QObject *parent)
    : WindowManager(parent) {
}

std::vector<std::shared_ptr<Window>> KDEWindowManagerBackend::listWindows() const {
    return {};
}

} // namespace surfacescry
