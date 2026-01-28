#include "EventLogModel.hpp"

namespace surfacescry {

EventLogModel::EventLogModel(QObject *parent)
    : QAbstractTableModel(parent) {
}

int EventLogModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 0;
}

int EventLogModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 3;
}

QVariant EventLogModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }
    return {};
}

QVariant EventLogModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case 0:
        return QStringLiteral("Time");
    case 1:
        return QStringLiteral("Event");
    case 2:
        return QStringLiteral("Details");
    default:
        return {};
    }
}

} // namespace surfacescry
