#pragma once

#include <QAbstractTableModel>

namespace surfacescry {

class EventLogModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit EventLogModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

} // namespace surfacescry
