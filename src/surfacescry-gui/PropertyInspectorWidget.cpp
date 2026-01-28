#include "PropertyInspectorWidget.hpp"

#include <QFormLayout>
#include <QLabel>

namespace surfacescry {

PropertyInspectorWidget::PropertyInspectorWidget(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QFormLayout(this);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    layout->addRow(tr("Title:"), new QLabel(tr("No window selected"), this));
    layout->addRow(tr("Application:"), new QLabel(tr("—"), this));
    layout->addRow(tr("PID:"), new QLabel(tr("—"), this));
    layout->addRow(tr("Backend:"), new QLabel(tr("—"), this));
    layout->addRow(tr("Geometry:"), new QLabel(tr("—"), this));
    layout->addRow(tr("State:"), new QLabel(tr("—"), this));
    layout->addRow(tr("Desktop:"), new QLabel(tr("—"), this));
}

} // namespace surfacescry
