#pragma once

#include <QMainWindow>

#include <memory>

#include "kde_window_manager_backend.hpp"

namespace surfacescry {

class EventLogView;
class PropertyInspectorWidget;
class WindowTreeModel;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onCurrentWindowChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    std::shared_ptr<KDEWindowManagerBackend> windowManager_;
    WindowTreeModel *windowModel_ = nullptr;
    PropertyInspectorWidget *propertyInspector_ = nullptr;
    EventLogView *eventLogView_ = nullptr;
};

} // namespace surfacescry
