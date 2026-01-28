#pragma once

#include <QAbstractTableModel>

#include <memory>
#include <vector>

#include "kde_window_manager_backend.hpp"

namespace surfacescry {

class WindowTreeModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit WindowTreeModel(std::shared_ptr<WindowManager> windowManager, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    void loadInitialWindows();
    int indexForWindowId(const QString &id) const;
    QString backendLabel(WindowBackend backend) const;

    std::shared_ptr<WindowManager> windowManager_;
    std::vector<std::shared_ptr<Window>> windows_;
};

} // namespace surfacescry
