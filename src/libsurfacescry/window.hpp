#pragma once

#include <QObject>

#include "window_types.hpp"

namespace surfacescry {

class Window : public QObject {
    Q_OBJECT

public:
    explicit Window(WindowInfo info, QObject *parent = nullptr);
    ~Window() override = default;

    const WindowInfo &info() const;
    void updateInfo(WindowInfo info);

protected:
    WindowInfo info_;
};

} // namespace surfacescry
