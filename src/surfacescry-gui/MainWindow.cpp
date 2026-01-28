#include "MainWindow.hpp"

#include <QHeaderView>
#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>

#include "EventLogView.hpp"
#include "PropertyInspectorWidget.hpp"
#include "WindowTreeModel.hpp"

namespace surfacescry {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , windowManager_(std::make_shared<KDEWindowManagerBackend>()) {
    auto *mainSplitter = new QSplitter(Qt::Horizontal, this);

    auto *windowTree = new QTreeView(mainSplitter);
    windowModel_ = new WindowTreeModel(windowManager_, windowTree);
    windowTree->setModel(windowModel_);
    windowTree->setHeaderHidden(false);
    windowTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    windowTree->setAlternatingRowColors(true);
    if (auto *header = windowTree->header()) {
        header->setSectionResizeMode(QHeaderView::ResizeToContents);
        header->setStretchLastSection(true);
    }

    auto *rightSplitter = new QSplitter(Qt::Vertical, mainSplitter);

    propertyInspector_ = new PropertyInspectorWidget(rightSplitter);
    eventLogView_ = new EventLogView(rightSplitter);

    rightSplitter->setStretchFactor(0, 2);
    rightSplitter->setStretchFactor(1, 1);

    mainSplitter->addWidget(windowTree);
    mainSplitter->addWidget(rightSplitter);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 2);

    setCentralWidget(mainSplitter);
}

} // namespace surfacescry
