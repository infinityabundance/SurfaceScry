#pragma once

#include <QHash>
#include <QString>
#include <QVector>

#include <KWindowInfo>
#include <KWindowSystem>

#include "window_manager.hpp"

namespace surfacescry {

class KDEWindowManagerBackend final : public WindowManager {
    Q_OBJECT

public:
    explicit KDEWindowManagerBackend(QObject *parent = nullptr);

    std::vector<std::shared_ptr<Window>> listWindows() const override;

private:
    void initializeWindows();
    void handleWindowAdded(WId id);
    void handleWindowRemoved(WId id);
    void handleWindowChanged(WId id, NET::Properties properties, NET::Properties2 properties2);
    void handleActiveWindowChanged(WId id);

    std::shared_ptr<Window> createWindow(WId id, int zOrder);
    WindowInfo buildWindowInfo(WId id, int zOrder) const;
    std::vector<WindowState> buildStates(const KWindowInfo &info) const;
    WindowBackend platformBackend() const;
    QString windowIdString(WId id) const;
    int indexForWindowId(WId id) const;
    void refreshZOrder();

    QHash<WId, std::shared_ptr<Window>> windows_;
    QVector<WId> windowOrder_;
    QString activeWindowId_;
};

} // namespace surfacescry
