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
    auto *windowSystem = KWindowSystem::self();
    if (windowSystem) {
        connect(windowSystem, &KWindowSystem::windowAdded, this, [this](WId id) { handleWindowAdded(id); });
        connect(windowSystem, &KWindowSystem::windowRemoved, this, [this](WId id) { handleWindowRemoved(id); });
        connect(windowSystem,
                &KWindowSystem::windowChanged,
                this,
                [this](WId id, NET::Properties properties, NET::Properties2 properties2) {
                    handleWindowChanged(id, properties, properties2);
                });
        connect(windowSystem, &KWindowSystem::activeWindowChanged, this, [this](WId id) {
            handleActiveWindowChanged(id);
        });
    }

    initializeWindows();
}

std::vector<std::shared_ptr<Window>> KDEWindowManagerBackend::listWindows() const {
    std::vector<std::shared_ptr<Window>> result;
    result.reserve(windowOrder_.size());
    for (WId id : windowOrder_) {
        if (auto it = windows_.find(id); it != windows_.end()) {
            result.push_back(it.value());
        }
    }
    return result;
}

void KDEWindowManagerBackend::initializeWindows() {
    const QList<WId> windows = KWindowSystem::windows();
    windowOrder_.clear();
    windows_.clear();
    windowOrder_.reserve(windows.size());

    int index = 0;
    for (WId id : windows) {
        auto window = createWindow(id, index++);
        if (window) {
            windowOrder_.append(id);
        }
    }

    activeWindowId_ = windowIdString(KWindowSystem::activeWindow());
}

void KDEWindowManagerBackend::handleWindowAdded(WId id) {
    if (windows_.contains(id)) {
        return;
    }

    const int zOrder = windowOrder_.size();
    auto window = createWindow(id, zOrder);
    if (!window) {
        return;
    }

    windowOrder_.append(id);
    emit windowCreated(window);
}

void KDEWindowManagerBackend::handleWindowRemoved(WId id) {
    auto it = windows_.find(id);
    if (it == windows_.end()) {
        return;
    }

    const QString idString = windowIdString(id);
    windows_.erase(it);

    const int index = indexForWindowId(id);
    if (index >= 0) {
        windowOrder_.removeAt(index);
    }

    refreshZOrder();
    emit windowDestroyed(idString);
}

void KDEWindowManagerBackend::handleWindowChanged(WId id,
                                                  NET::Properties properties,
                                                  NET::Properties2 properties2) {
    auto it = windows_.find(id);
    if (it == windows_.end()) {
        return;
    }

    const int zOrder = indexForWindowId(id);
    WindowInfo info = buildWindowInfo(id, zOrder);
    it.value()->updateInfo(std::move(info));

    QStringList fields;
    if (properties & (NET::WMName | NET::WMVisibleName)) {
        fields << QStringLiteral("title");
    }
    if (properties & NET::WMPid) {
        fields << QStringLiteral("pid");
    }
    if (properties & NET::WMGeometry) {
        fields << QStringLiteral("geometry");
    }
    if (properties & NET::WMState) {
        fields << QStringLiteral("states");
    }
    if (properties & NET::WMDesktop) {
        fields << QStringLiteral("desktop");
    }
    if (properties2 & NET::WM2WindowClass) {
        fields << QStringLiteral("appName");
    }
    if (properties2 & NET::WM2DesktopFileName) {
        fields << QStringLiteral("appId");
    }

    emit windowUpdated(it.value(), fields);
}

void KDEWindowManagerBackend::handleActiveWindowChanged(WId id) {
    const QString newId = windowIdString(id);
    if (newId == activeWindowId_) {
        return;
    }

    const QString oldId = activeWindowId_;
    activeWindowId_ = newId;
    emit focusChanged(newId, oldId);
}

std::shared_ptr<Window> KDEWindowManagerBackend::createWindow(WId id, int zOrder) {
    WindowInfo info = buildWindowInfo(id, zOrder);
    auto window = std::make_shared<Window>(std::move(info));
    windows_.insert(id, window);
    return window;
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
    if (state & NET::Hidden) {
        states.push_back(WindowState::Hidden);
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

int KDEWindowManagerBackend::indexForWindowId(WId id) const {
    for (int i = 0; i < windowOrder_.size(); ++i) {
        if (windowOrder_.at(i) == id) {
            return i;
        }
    }
    return -1;
}

void KDEWindowManagerBackend::refreshZOrder() {
    for (int i = 0; i < windowOrder_.size(); ++i) {
        const WId id = windowOrder_.at(i);
        auto it = windows_.find(id);
        if (it == windows_.end()) {
            continue;
        }
        WindowInfo info = it.value()->info();
        info.zOrder = i;
        it.value()->updateInfo(std::move(info));
    }
}

} // namespace surfacescry
