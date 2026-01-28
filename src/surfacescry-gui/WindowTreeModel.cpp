#include "WindowTreeModel.hpp"

#include <QBrush>
#include <QColor>
#include <QFont>

namespace surfacescry {

WindowTreeModel::WindowTreeModel(std::shared_ptr<WindowManager> windowManager, QObject *parent)
    : QAbstractTableModel(parent)
    , windowManager_(std::move(windowManager)) {
    if (windowManager_) {
        windows_ = windowManager_->listWindows();

        connect(windowManager_.get(), &WindowManager::windowCreated, this, &WindowTreeModel::onWindowCreated);
        connect(windowManager_.get(), &WindowManager::windowDestroyed, this, &WindowTreeModel::onWindowDestroyed);
        connect(windowManager_.get(), &WindowManager::windowUpdated, this, &WindowTreeModel::onWindowUpdated);
        connect(windowManager_.get(), &WindowManager::focusChanged, this, &WindowTreeModel::onFocusChanged);
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
    if (!index.isValid()) {
        return {};
    }

    const int row = index.row();
    if (row < 0 || row >= static_cast<int>(windows_.size())) {
        return {};
    }

    const WindowInfo &info = windows_[row]->info();

    if (role == Qt::DisplayRole) {
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

    if (!activeWindowId_.isEmpty() && info.id == activeWindowId_.toStdString()) {
        if (role == Qt::FontRole) {
            QFont font;
            font.setBold(true);
            return font;
        }
        if (role == Qt::BackgroundRole) {
            return QBrush(QColor(60, 90, 140, 80));
        }
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

std::shared_ptr<Window> WindowTreeModel::windowForIndex(const QModelIndex &index) const {
    if (!index.isValid()) {
        return nullptr;
    }

    const int row = index.row();
    if (row < 0 || row >= static_cast<int>(windows_.size())) {
        return nullptr;
    }

    return windows_[row];
}

void WindowTreeModel::onWindowCreated(std::shared_ptr<Window> window) {
    const int row = static_cast<int>(windows_.size());
    beginInsertRows(QModelIndex(), row, row);
    windows_.push_back(std::move(window));
    endInsertRows();
}

void WindowTreeModel::onWindowDestroyed(const QString &id) {
    const int row = indexForId(id);
    if (row < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    windows_.erase(windows_.begin() + row);
    endRemoveRows();
}

void WindowTreeModel::onWindowUpdated(std::shared_ptr<Window> window, const QStringList &) {
    const QString id = QString::fromStdString(window->info().id);
    const int row = indexForId(id);
    if (row < 0) {
        return;
    }

    emit dataChanged(index(row, 0), index(row, columnCount() - 1), {Qt::DisplayRole});
}

void WindowTreeModel::onFocusChanged(const QString &newId, const QString &oldId) {
    Q_UNUSED(oldId);
    if (activeWindowId_ == newId) {
        return;
    }

    const QString previous = activeWindowId_;
    activeWindowId_ = newId;

    auto markRow = [this](const QString &id) {
        if (id.isEmpty()) {
            return;
        }
        const std::string sid = id.toStdString();
        for (int row = 0; row < static_cast<int>(windows_.size()); ++row) {
            if (windows_[row]->info().id == sid) {
                emit dataChanged(index(row, 0),
                                 index(row, columnCount() - 1),
                                 {Qt::BackgroundRole, Qt::FontRole});
                break;
            }
        }
    };

    markRow(previous);
    markRow(activeWindowId_);
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

int WindowTreeModel::indexForId(const QString &id) const {
    for (int i = 0; i < static_cast<int>(windows_.size()); ++i) {
        if (QString::fromStdString(windows_[i]->info().id) == id) {
            return i;
        }
    }
    return -1;
}

WindowFilterProxyModel::WindowFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {
    setDynamicSortFilter(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void WindowFilterProxyModel::setFilterText(const QString &text) {
    if (filterText_ == text) {
        return;
    }
    filterText_ = text;
    invalidateFilter();
}

void WindowFilterProxyModel::setBackendFilter(std::optional<WindowBackend> backend) {
    backendFilter_ = backend;
    invalidateFilter();
}

bool WindowFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    if (!sourceModel()) {
        return true;
    }

    const QModelIndex titleIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    const QModelIndex appIndex = sourceModel()->index(sourceRow, 1, sourceParent);
    const QModelIndex pidIndex = sourceModel()->index(sourceRow, 2, sourceParent);
    const QModelIndex backendIndex = sourceModel()->index(sourceRow, 3, sourceParent);

    const QString title = sourceModel()->data(titleIndex, Qt::DisplayRole).toString();
    const QString app = sourceModel()->data(appIndex, Qt::DisplayRole).toString();
    const QString pidStr = sourceModel()->data(pidIndex, Qt::DisplayRole).toString();
    const QString backend = sourceModel()->data(backendIndex, Qt::DisplayRole).toString();

    if (backendFilter_.has_value()) {
        const QString wanted = [this]() {
            switch (*backendFilter_) {
            case WindowBackend::X11:
                return QStringLiteral("X11");
            case WindowBackend::Wayland:
                return QStringLiteral("Wayland");
            case WindowBackend::Unknown:
            default:
                return QStringLiteral("Unknown");
            }
        }();

        if (backend.compare(wanted, Qt::CaseInsensitive) != 0) {
            return false;
        }
    }

    if (filterText_.isEmpty()) {
        return true;
    }

    const QString haystack = title + QLatin1Char(' ') + app + QLatin1Char(' ') + pidStr + QLatin1Char(' ') + backend;
    return haystack.contains(filterText_, Qt::CaseInsensitive);
}

} // namespace surfacescry
