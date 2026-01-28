#pragma once

#include <QMainWindow>

#include <memory>

#include "kde_window_manager_backend.hpp"

class QComboBox;
class QLineEdit;

namespace surfacescry {

class EventLogView;
class PropertyInspectorWidget;
class WindowFilterProxyModel;
class WindowTreeModel;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onCurrentWindowChanged(const QModelIndex &current, const QModelIndex &previous);
    void updateStatusBarCounts();

private:
    std::shared_ptr<KDEWindowManagerBackend> windowManager_;
    WindowTreeModel *windowModel_ = nullptr;
    WindowFilterProxyModel *windowProxyModel_ = nullptr;
    PropertyInspectorWidget *propertyInspector_ = nullptr;
    EventLogView *eventLogView_ = nullptr;
    QLineEdit *searchEdit_ = nullptr;
    QComboBox *backendFilterCombo_ = nullptr;
};

} // namespace surfacescry
