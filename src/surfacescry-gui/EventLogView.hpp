#pragma once

#include <QWidget>

#include <memory>

#include "window_manager.hpp"

namespace surfacescry {

class EventLogModel;

class EventLogView final : public QWidget {
    Q_OBJECT

public:
    explicit EventLogView(std::shared_ptr<WindowManager> windowManager, QWidget *parent = nullptr);

private:
    EventLogModel *model_ = nullptr;
};

} // namespace surfacescry
