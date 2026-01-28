#pragma once

#include <QLabel>
#include <QWidget>

#include "window_types.hpp"

namespace surfacescry {

class PropertyInspectorWidget final : public QWidget {
    Q_OBJECT

public:
    explicit PropertyInspectorWidget(QWidget *parent = nullptr);

    void clear();
    void setWindow(const WindowInfo &info);

private:
    QLabel *titleLabel_ = nullptr;
    QLabel *appLabel_ = nullptr;
    QLabel *pidLabel_ = nullptr;
    QLabel *backendLabel_ = nullptr;
    QLabel *geometryLabel_ = nullptr;
    QLabel *stateLabel_ = nullptr;
    QLabel *desktopLabel_ = nullptr;
};

} // namespace surfacescry
