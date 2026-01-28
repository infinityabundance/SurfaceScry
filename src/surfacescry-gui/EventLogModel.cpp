#include "EventLogModel.hpp"

namespace surfacescry {

EventLogModel::EventLogModel(std::shared_ptr<WindowManager> windowManager, QObject *parent)
    : QAbstractTableModel(parent)
    , windowManager_(std::move(windowManager)) {
    if (windowManager_) {
        connect(windowManager_.get(), &WindowManager::windowCreated, this, &EventLogModel::onWindowCreated);
        connect(windowManager_.get(), &WindowManager::windowDestroyed, this, &EventLogModel::onWindowDestroyed);
        connect(windowManager_.get(), &WindowManager::windowUpdated, this, &EventLogModel::onWindowUpdated);
        connect(windowManager_.get(), &WindowManager::focusChanged, this, &EventLogModel::onFocusChanged);
    }
}

int EventLogModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return events_.size();
}

int EventLogModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 4;
}

QVariant EventLogModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }

    const int row = index.row();
    if (row < 0 || row >= events_.size()) {
        return {};
    }

    const EventEntry &entry = events_.at(row);
    switch (index.column()) {
    case 0:
        return entry.timestamp.toString(Qt::ISODate);
    case 1:
        return entry.eventType;
    case 2:
        return entry.windowTitle;
    case 3:
        return entry.details;
    default:
        return {};
    }
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
        return QStringLiteral("Title");
    case 3:
        return QStringLiteral("Details");
    default:
        return {};
    }
}

void EventLogModel::appendEvent(const QString &eventType,
                                const QString &windowId,
                                const QString &windowTitle,
                                const QString &details) {
    EventEntry entry{QDateTime::currentDateTime(), eventType, windowId, windowTitle, details};

    const int row = events_.size();
    beginInsertRows(QModelIndex(), row, row);
    events_.append(entry);
    endInsertRows();

    if (events_.size() > maxEvents_) {
        beginRemoveRows(QModelIndex(), 0, 0);
        events_.removeFirst();
        endRemoveRows();
    }
}

void EventLogModel::onWindowCreated(std::shared_ptr<Window> window) {
    const WindowInfo &info = window->info();
    appendEvent(QStringLiteral("created"),
                QString::fromStdString(info.id),
                QString::fromStdString(info.title),
                QString());
}

void EventLogModel::onWindowDestroyed(const QString &id) {
    appendEvent(QStringLiteral("destroyed"), id, QString(), QString());
}

void EventLogModel::onWindowUpdated(std::shared_ptr<Window> window, const QStringList &fields) {
    const WindowInfo &info = window->info();
    appendEvent(QStringLiteral("updated"),
                QString::fromStdString(info.id),
                QString::fromStdString(info.title),
                fields.join(QStringLiteral(", ")));
}

void EventLogModel::onFocusChanged(const QString &newId, const QString &oldId) {
    appendEvent(QStringLiteral("focusChanged"),
                newId,
                QString(),
                QStringLiteral("from %1 to %2").arg(oldId, newId));
}

} // namespace surfacescry
