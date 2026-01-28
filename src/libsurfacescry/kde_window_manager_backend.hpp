#pragma once

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
    WindowBackend platformBackend() const;
    QString windowIdString(WId id) const;

    std::vector<std::shared_ptr<Window>> windows_;
};

} // namespace surfacescry
