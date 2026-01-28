#include "kde_window_manager_backend.hpp"

#include <QRect>
#include <QStringList>

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

    const QList<WId> windowIds = KWindowSystem::windows();
    windows_.reserve(windowIds.size());

    for (int i = 0; i < windowIds.size(); ++i) {
        const WId id = windowIds.at(i);
        WindowInfo info = buildWindowInfo(id, i);
        auto window = std::make_shared<Window>(std::move(info));
        windows_.push_back(window);
        windowsById_.insert(id, window);
    }

    activeWindowId_ = KWindowSystem::activeWindow();
}

std::vector<std::shared_ptr<Window>> KDEWindowManagerBackend::listWindows() const {
    return windows_;
}

WindowInfo KDEWindowManagerBackend::buildWindowInfo(WId id, int zOrder) const {
    KWindowInfo info(id, kProperties, kProperties2);

    WindowInfo windowInfo;
    windowInfo.id = idToString(id).toStdString();

    const QString visibleName = info.visibleName();
    const QString title = !visibleName.isEmpty() ? visibleName : info.name();
    windowInfo.title = title.toStdString();

    const QString className = info.windowClassClass();
    const QString classInstance = info.windowClassName();
    const QString appName = !className.isEmpty() ? className : classInstance;
    windowInfo.appName = appName.toStdString();
    windowInfo.appId = info.desktopFileName().toStdString();

    windowInfo.pid = static_cast<pid_t>(info.pid());
    windowInfo.backend = detectBackend();

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

WindowBackend KDEWindowManagerBackend::detectBackend() const {
    if (KWindowSystem::isPlatformX11()) {
        return WindowBackend::X11;
    }
    if (KWindowSystem::isPlatformWayland()) {
        return WindowBackend::Wayland;
    }
    return WindowBackend::Unknown;
}

QString KDEWindowManagerBackend::idToString(WId id) const {
    switch (detectBackend()) {
    case WindowBackend::X11:
        return QStringLiteral("x11:0x%1").arg(static_cast<qulonglong>(id), 0, 16);
    case WindowBackend::Wayland:
        return QStringLiteral("wayland:%1").arg(static_cast<qulonglong>(id));
    case WindowBackend::Unknown:
    default:
        return QStringLiteral("unknown:%1").arg(static_cast<qulonglong>(id));
    }
}

int KDEWindowManagerBackend::zOrderForId(WId id) const {
    const QList<WId> current = KWindowSystem::windows();
    return current.indexOf(id);
}

void KDEWindowManagerBackend::handleWindowAdded(WId id) {
    if (windowsById_.contains(id)) {
        return;
    }

    const int zOrder = zOrderForId(id);
    WindowInfo info = buildWindowInfo(id, zOrder >= 0 ? zOrder : static_cast<int>(windows_.size()));
    auto window = std::make_shared<Window>(std::move(info));

    windows_.push_back(window);
    windowsById_.insert(id, window);
    emit windowCreated(window);
}

void KDEWindowManagerBackend::handleWindowRemoved(WId id) {
    auto it = windowsById_.find(id);
    if (it == windowsById_.end()) {
        return;
    }

    const QString idString = idToString(id);
    const std::string idStd = idString.toStdString();
    windowsById_.erase(it);

    for (auto vecIt = windows_.begin(); vecIt != windows_.end(); ++vecIt) {
        if ((*vecIt)->info().id == idStd) {
            windows_.erase(vecIt);
            break;
        }
    }

    emit windowDestroyed(idString);
}

void KDEWindowManagerBackend::handleWindowChanged(WId id,
                                                  NET::Properties properties,
                                                  NET::Properties2 properties2) {
    auto it = windowsById_.find(id);
    if (it == windowsById_.end()) {
        return;
    }

    const int zOrder = zOrderForId(id);
    WindowInfo info = buildWindowInfo(id, zOrder >= 0 ? zOrder : it.value()->info().zOrder);
    it.value()->updateInfo(std::move(info));

    QStringList fields;
    if (properties & (NET::WMName | NET::WMVisibleName)) {
        fields << QStringLiteral("title");
    }
    if (properties & NET::WMGeometry) {
        fields << QStringLiteral("geometry");
    }
    if (properties & NET::WMState) {
        fields << QStringLiteral("state");
    }
    if (properties & NET::WMDesktop) {
        fields << QStringLiteral("desktop");
    }
    if (properties & NET::WMPid) {
        fields << QStringLiteral("pid");
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
    const QString newId = idToString(id);
    const QString oldId = idToString(activeWindowId_);

    if (id == activeWindowId_) {
        return;
    }

    activeWindowId_ = id;
    emit focusChanged(newId, oldId);
}

} // namespace surfacescry
