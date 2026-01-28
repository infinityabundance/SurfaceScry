#pragma once

#include <QWidget>

namespace surfacescry {

class EventLogModel;

class EventLogView final : public QWidget {
    Q_OBJECT

public:
    explicit EventLogView(QWidget *parent = nullptr);

private:
    EventLogModel *model_ = nullptr;
};

} // namespace surfacescry
