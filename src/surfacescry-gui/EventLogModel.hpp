#pragma once

#include <QAbstractTableModel>
#include <QDateTime>
#include <QStringList>
#include <QVector>

#include <memory>

#include "window_manager.hpp"

namespace surfacescry {

class EventLogModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit EventLogModel(std::shared_ptr<WindowManager> windowManager, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private slots:
    void onWindowCreated(std::shared_ptr<Window> window);
    void onWindowDestroyed(const QString &id);
    void onWindowUpdated(std::shared_ptr<Window> window, const QStringList &fields);
    void onFocusChanged(const QString &newId, const QString &oldId);

private:
    struct EventEntry {
        QDateTime timestamp;
        QString eventType;
        QString windowId;
        QString windowTitle;
        QString details;
    };

    void appendEvent(const QString &eventType,
                     const QString &windowId,
                     const QString &windowTitle,
                     const QString &details);

    std::shared_ptr<WindowManager> windowManager_;
    QVector<EventEntry> events_;
    int maxEvents_ = 500;
};

} // namespace surfacescry
