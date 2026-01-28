#include "kde_window_manager_backend.hpp"

#include <QRect>

namespace surfacescry {

namespace {
constexpr NET::Properties kProperties = NET::WMName | NET::WMVisibleName | NET::WMGeometry | NET::WMState |
    NET::WMDesktop | NET::WMPid;
constexpr NET::Properties2 kProperties2 = NET::WM2WindowClass | NET::WM2DesktopFileName;
} // namespace

KDEWindowManagerBackend::KDEWindowManagerBackend(QObject *parent)
    : WindowManager(parent) {
    const QList<WId> windowIds = KWindowSystem::windows();
    windows_.reserve(windowIds.size());

    int zOrder = 0;
    for (WId id : windowIds) {
        WindowInfo info = buildWindowInfo(id, zOrder++);
        windows_.push_back(std::make_shared<Window>(std::move(info)));
    }
}

std::vector<std::shared_ptr<Window>> KDEWindowManagerBackend::listWindows() const {
    return windows_;
}

WindowInfo KDEWindowManagerBackend::buildWindowInfo(WId id, int zOrder) const {
    KWindowInfo info(id, kProperties, kProperties2);

    WindowInfo windowInfo;
    windowInfo.id = windowIdString(id).toStdString();

    const QString visibleName = info.visibleName();
    const QString title = !visibleName.isEmpty() ? visibleName : info.name();
    windowInfo.title = title.toStdString();

    const QString className = info.windowClassClass();
    const QString classInstance = info.windowClassName();
    const QString appName = !className.isEmpty() ? className : classInstance;
    windowInfo.appName = appName.toStdString();
    windowInfo.appId = info.desktopFileName().toStdString();

    windowInfo.pid = static_cast<pid_t>(info.pid());
    windowInfo.backend = platformBackend();

    const QRect geometry = info.geometry();
    windowInfo.geometry = WindowGeometry{geometry.x(), geometry.y(), geometry.width(), geometry.height()};

    windowInfo.states = buildStates(info);

    const int desktop = info.desktop();
    windowInfo.desktopIndex = desktop > 0 ? desktop : -1;
    windowInfo.zOrder = zOrder;

    return windowInfo;
}

std::vector<WindowState> KDEWindowManagerBackend::buildStates(const KWindowInfo &info) const {
    const NET::States state = info.state();
    std::vector<WindowState> states;

    if (state & NET::Active) {
        states.push_back(WindowState::Active);
    }
    if (state & NET::Minimized) {
        states.push_back(WindowState::Minimized);
    }
    if (state & NET::Max) {
        states.push_back(WindowState::Maximized);
    }
    if (state & NET::FullScreen) {
        states.push_back(WindowState::Fullscreen);
    }

    if (states.empty()) {
        states.push_back(WindowState::Normal);
    }

    return states;
}

WindowBackend KDEWindowManagerBackend::platformBackend() const {
    if (KWindowSystem::isPlatformWayland()) {
        return WindowBackend::Wayland;
    }
    if (KWindowSystem::isPlatformX11()) {
        return WindowBackend::X11;
    }
    return WindowBackend::Unknown;
}

QString KDEWindowManagerBackend::windowIdString(WId id) const {
    switch (platformBackend()) {
    case WindowBackend::X11:
        return QStringLiteral("x11:0x%1").arg(static_cast<qulonglong>(id), 0, 16);
    case WindowBackend::Wayland:
        return QStringLiteral("wayland:%1").arg(static_cast<qulonglong>(id));
    case WindowBackend::Unknown:
    default:
        return QStringLiteral("unknown:%1").arg(static_cast<qulonglong>(id));
    }
}

} // namespace surfacescry
