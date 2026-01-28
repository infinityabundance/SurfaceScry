#pragma once

#include <QAbstractTableModel>

#include <memory>

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
    std::shared_ptr<WindowManager> windowManager_;
};

} // namespace surfacescry
