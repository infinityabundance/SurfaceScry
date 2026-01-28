#include "MainWindow.hpp"

#include <QComboBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QTreeView>
#include <QVBoxLayout>

#include <optional>

#include "EventLogView.hpp"
#include "PropertyInspectorWidget.hpp"
#include "WindowTreeModel.hpp"

namespace surfacescry {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , windowManager_(std::make_shared<KDEWindowManagerBackend>()) {
    auto *mainSplitter = new QSplitter(Qt::Horizontal, this);

    auto *leftContainer = new QWidget(mainSplitter);
    auto *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    auto *searchRow = new QHBoxLayout();
    searchEdit_ = new QLineEdit(leftContainer);
    searchEdit_->setPlaceholderText(tr("Filter windows..."));

    backendFilterCombo_ = new QComboBox(leftContainer);
    backendFilterCombo_->addItem(tr("All"));
    backendFilterCombo_->addItem(tr("X11"));
    backendFilterCombo_->addItem(tr("Wayland"));
    backendFilterCombo_->addItem(tr("Unknown"));

    searchRow->addWidget(searchEdit_, 1);
    searchRow->addWidget(backendFilterCombo_, 0);
    leftLayout->addLayout(searchRow);

    auto *windowTree = new QTreeView(leftContainer);
    windowModel_ = new WindowTreeModel(windowManager_, windowTree);

    windowProxyModel_ = new WindowFilterProxyModel(windowTree);
    windowProxyModel_->setSourceModel(windowModel_);
    windowProxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);

    windowTree->setModel(windowProxyModel_);
    windowTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    windowTree->setSelectionMode(QAbstractItemView::SingleSelection);
    windowTree->setRootIsDecorated(false);
    windowTree->setAlternatingRowColors(true);
    if (auto *header = windowTree->header()) {
        header->setSectionResizeMode(QHeaderView::ResizeToContents);
        header->setStretchLastSection(true);
    }

    leftLayout->addWidget(windowTree, 1);

    auto *rightSplitter = new QSplitter(Qt::Vertical, mainSplitter);

    propertyInspector_ = new PropertyInspectorWidget(rightSplitter);
    eventLogView_ = new EventLogView(windowManager_, rightSplitter);

    rightSplitter->setStretchFactor(0, 2);
    rightSplitter->setStretchFactor(1, 1);

    mainSplitter->addWidget(leftContainer);
    mainSplitter->addWidget(rightSplitter);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 2);

    setCentralWidget(mainSplitter);

    connect(searchEdit_, &QLineEdit::textChanged, windowProxyModel_, &WindowFilterProxyModel::setFilterText);
    connect(backendFilterCombo_, &QComboBox::currentIndexChanged, this, [this](int index) {
        std::optional<WindowBackend> backend;
        switch (index) {
        case 1:
            backend = WindowBackend::X11;
            break;
        case 2:
            backend = WindowBackend::Wayland;
            break;
        case 3:
            backend = WindowBackend::Unknown;
            break;
        default:
            backend.reset();
            break;
        }
        windowProxyModel_->setBackendFilter(backend);
    });

    if (auto *selectionModel = windowTree->selectionModel()) {
        connect(selectionModel,
                &QItemSelectionModel::currentChanged,
                this,
                &MainWindow::onCurrentWindowChanged);
    }

    connect(windowModel_, &QAbstractItemModel::modelReset, this, &MainWindow::updateStatusBarCounts);
    connect(windowModel_, &QAbstractItemModel::rowsInserted, this, &MainWindow::updateStatusBarCounts);
    connect(windowModel_, &QAbstractItemModel::rowsRemoved, this, &MainWindow::updateStatusBarCounts);
    connect(windowProxyModel_, &QAbstractItemModel::modelReset, this, &MainWindow::updateStatusBarCounts);
    connect(windowProxyModel_, &QAbstractItemModel::rowsInserted, this, &MainWindow::updateStatusBarCounts);
    connect(windowProxyModel_, &QAbstractItemModel::rowsRemoved, this, &MainWindow::updateStatusBarCounts);

    statusBar()->showMessage(tr("Ready"));
    updateStatusBarCounts();
}

void MainWindow::onCurrentWindowChanged(const QModelIndex &current, const QModelIndex &) {
    if (!current.isValid()) {
        propertyInspector_->clear();
        return;
    }

    if (!windowProxyModel_ || !windowModel_) {
        propertyInspector_->clear();
        return;
    }

    const QModelIndex sourceIndex = windowProxyModel_->mapToSource(current);
    auto window = windowModel_->windowForIndex(sourceIndex);
    if (!window) {
        propertyInspector_->clear();
        return;
    }

    propertyInspector_->setWindow(window->info());
}

void MainWindow::updateStatusBarCounts() {
    if (!windowModel_ || !windowProxyModel_) {
        statusBar()->clearMessage();
        return;
    }

    const int total = windowModel_->rowCount();
    const int visible = windowProxyModel_->rowCount();
    statusBar()->showMessage(tr("%1 windows (%2 visible)").arg(total).arg(visible));
}

} // namespace surfacescry
