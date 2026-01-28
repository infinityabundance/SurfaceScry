#include "EventLogView.hpp"

#include <QHeaderView>
#include <QTableView>
#include <QVBoxLayout>

#include "EventLogModel.hpp"

namespace surfacescry {

EventLogView::EventLogView(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    auto *view = new QTableView(this);
    model_ = new EventLogModel(view);
    view->setModel(model_);
    view->horizontalHeader()->setStretchLastSection(true);
    view->verticalHeader()->setVisible(false);

    layout->addWidget(view);
}

} // namespace surfacescry
