#pragma once

#include <QHash>
#include <vector>

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
    WindowInfo buildWindowInfo(WId id, int zOrder) const;
    std::vector<WindowState> buildStates(const KWindowInfo &info) const;
    WindowBackend detectBackend() const;
    QString idToString(WId id) const;
    int zOrderForId(WId id) const;

    void handleWindowAdded(WId id);
    void handleWindowRemoved(WId id);
    void handleWindowChanged(WId id, NET::Properties properties, NET::Properties2 properties2);
    void handleActiveWindowChanged(WId id);

    std::vector<std::shared_ptr<Window>> windows_;
    QHash<WId, std::shared_ptr<Window>> windowsById_;
    WId activeWindowId_ = 0;
};

} // namespace surfacescry
