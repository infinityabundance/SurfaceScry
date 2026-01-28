#include "WindowTreeModel.hpp"

namespace surfacescry {

WindowTreeModel::WindowTreeModel(std::shared_ptr<WindowManager> windowManager, QObject *parent)
    : QAbstractTableModel(parent)
    , windowManager_(std::move(windowManager)) {
    loadInitialWindows();

    if (windowManager_) {
        auto *manager = windowManager_.get();
        connect(manager, &WindowManager::windowCreated, this, [this](std::shared_ptr<Window> window) {
            const int row = static_cast<int>(windows_.size());
            beginInsertRows(QModelIndex(), row, row);
            windows_.push_back(std::move(window));
            endInsertRows();
        });

        connect(manager, &WindowManager::windowDestroyed, this, [this](const QString &id) {
            const int row = indexForWindowId(id);
            if (row < 0) {
                return;
            }
            beginRemoveRows(QModelIndex(), row, row);
            windows_.erase(windows_.begin() + row);
            endRemoveRows();
        });

        connect(manager,
                &WindowManager::windowUpdated,
                this,
                [this](std::shared_ptr<Window> window, const QStringList &) {
                    const QString id = QString::fromStdString(window->info().id);
                    const int row = indexForWindowId(id);
                    if (row < 0) {
                        return;
                    }
                    const QModelIndex topLeft = index(row, 0);
                    const QModelIndex bottomRight = index(row, columnCount() - 1);
                    emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});
                });
    }
}

int WindowTreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(windows_.size());
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

    const int row = index.row();
    if (row < 0 || row >= static_cast<int>(windows_.size())) {
        return {};
    }

    const WindowInfo &info = windows_[row]->info();

    switch (index.column()) {
    case 0:
        return QString::fromStdString(info.title);
    case 1: {
        const QString appName = QString::fromStdString(info.appName);
        const QString appId = QString::fromStdString(info.appId);
        return !appName.isEmpty() ? appName : appId;
    }
    case 2:
        return info.pid > 0 ? QString::number(info.pid) : QStringLiteral("-");
    case 3:
        return backendLabel(info.backend);
    default:
        return {};
    }
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

void WindowTreeModel::loadInitialWindows() {
    beginResetModel();
    windows_.clear();
    if (windowManager_) {
        windows_ = windowManager_->listWindows();
    }
    endResetModel();
}

int WindowTreeModel::indexForWindowId(const QString &id) const {
    for (int i = 0; i < static_cast<int>(windows_.size()); ++i) {
        if (QString::fromStdString(windows_[i]->info().id) == id) {
            return i;
        }
    }
    return -1;
}

QString WindowTreeModel::backendLabel(WindowBackend backend) const {
    switch (backend) {
    case WindowBackend::X11:
        return QStringLiteral("X11");
    case WindowBackend::Wayland:
        return QStringLiteral("Wayland");
    case WindowBackend::Unknown:
    default:
        return QStringLiteral("Unknown");
    }
}

} // namespace surfacescry
