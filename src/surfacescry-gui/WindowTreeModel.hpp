#pragma once

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QStringList>

#include <optional>
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

    std::shared_ptr<Window> windowForIndex(const QModelIndex &index) const;

private slots:
    void onWindowCreated(std::shared_ptr<Window> window);
    void onWindowDestroyed(const QString &id);
    void onWindowUpdated(std::shared_ptr<Window> window, const QStringList &fields);
    void onFocusChanged(const QString &newId, const QString &oldId);

private:
    QString backendLabel(WindowBackend backend) const;
    int indexForId(const QString &id) const;

    std::shared_ptr<WindowManager> windowManager_;
    std::vector<std::shared_ptr<Window>> windows_;
    QString activeWindowId_;
};

class WindowFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit WindowFilterProxyModel(QObject *parent = nullptr);

    void setFilterText(const QString &text);
    void setBackendFilter(std::optional<WindowBackend> backend);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString filterText_;
    std::optional<WindowBackend> backendFilter_;
};

} // namespace surfacescry
