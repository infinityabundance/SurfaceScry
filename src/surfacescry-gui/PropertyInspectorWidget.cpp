#include "PropertyInspectorWidget.hpp"

#include <QFormLayout>
#include <QStringList>

namespace surfacescry {

namespace {
QString backendLabel(WindowBackend backend) {
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

QString stateLabel(const std::vector<WindowState> &states) {
    QStringList labels;
    labels.reserve(static_cast<int>(states.size()));
    for (WindowState state : states) {
        switch (state) {
        case WindowState::Active:
            labels << QStringLiteral("Active");
            break;
        case WindowState::Minimized:
            labels << QStringLiteral("Minimized");
            break;
        case WindowState::Maximized:
            labels << QStringLiteral("Maximized");
            break;
        case WindowState::Fullscreen:
            labels << QStringLiteral("Fullscreen");
            break;
        case WindowState::Hidden:
            labels << QStringLiteral("Hidden");
            break;
        case WindowState::Normal:
            labels << QStringLiteral("Normal");
            break;
        }
    }

    if (labels.isEmpty()) {
        return QStringLiteral("—");
    }

    return labels.join(QStringLiteral(", "));
}
} // namespace

PropertyInspectorWidget::PropertyInspectorWidget(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QFormLayout(this);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    titleLabel_ = new QLabel(this);
    appLabel_ = new QLabel(this);
    pidLabel_ = new QLabel(this);
    backendLabel_ = new QLabel(this);
    geometryLabel_ = new QLabel(this);
    stateLabel_ = new QLabel(this);
    desktopLabel_ = new QLabel(this);

    layout->addRow(tr("Title:"), titleLabel_);
    layout->addRow(tr("Application:"), appLabel_);
    layout->addRow(tr("PID:"), pidLabel_);
    layout->addRow(tr("Backend:"), backendLabel_);
    layout->addRow(tr("Geometry:"), geometryLabel_);
    layout->addRow(tr("State:"), stateLabel_);
    layout->addRow(tr("Desktop:"), desktopLabel_);

    clear();
}

void PropertyInspectorWidget::clear() {
    titleLabel_->setText(tr("No window selected"));
    appLabel_->setText(tr("—"));
    pidLabel_->setText(tr("—"));
    backendLabel_->setText(tr("—"));
    geometryLabel_->setText(tr("—"));
    stateLabel_->setText(tr("—"));
    desktopLabel_->setText(tr("—"));
}

void PropertyInspectorWidget::setWindow(const WindowInfo &info) {
    titleLabel_->setText(QString::fromStdString(info.title));

    const QString appName = QString::fromStdString(info.appName);
    const QString appId = QString::fromStdString(info.appId);
    appLabel_->setText(!appName.isEmpty() ? appName : appId);

    pidLabel_->setText(info.pid > 0 ? QString::number(info.pid) : QStringLiteral("—"));
    backendLabel_->setText(backendLabel(info.backend));

    geometryLabel_->setText(QStringLiteral("x=%1, y=%2, w=%3, h=%4")
                                .arg(info.geometry.x)
                                .arg(info.geometry.y)
                                .arg(info.geometry.width)
                                .arg(info.geometry.height));

    stateLabel_->setText(stateLabel(info.states));

    if (info.desktopIndex >= 0) {
        desktopLabel_->setText(QString::number(info.desktopIndex));
    } else {
        desktopLabel_->setText(QStringLiteral("—"));
    }
}

} // namespace surfacescry
