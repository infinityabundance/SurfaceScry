#pragma once

#include "window_manager.hpp"

namespace surfacescry {

class KDEWindowManagerBackend final : public WindowManager {
    Q_OBJECT

public:
    explicit KDEWindowManagerBackend(QObject *parent = nullptr);

    std::vector<std::shared_ptr<Window>> listWindows() const override;
};

} // namespace surfacescry
