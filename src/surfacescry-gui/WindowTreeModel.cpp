#include "WindowTreeModel.hpp"

namespace surfacescry {

WindowTreeModel::WindowTreeModel(std::shared_ptr<WindowManager> windowManager, QObject *parent)
    : QAbstractTableModel(parent)
    , windowManager_(std::move(windowManager)) {
}

int WindowTreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 0;
}

int WindowTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 4;
}

QVariant WindowTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }
    return {};
}

QVariant WindowTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case 0:
        return QStringLiteral("Title");
    case 1:
        return QStringLiteral("Application");
    case 2:
        return QStringLiteral("PID");
    case 3:
        return QStringLiteral("Backend");
    default:
        return {};
    }
}

} // namespace surfacescry
