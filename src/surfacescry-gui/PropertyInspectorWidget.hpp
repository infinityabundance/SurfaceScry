#pragma once

#include <QWidget>

namespace surfacescry {

class PropertyInspectorWidget final : public QWidget {
    Q_OBJECT

public:
    explicit PropertyInspectorWidget(QWidget *parent = nullptr);
};

} // namespace surfacescry
