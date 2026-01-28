#pragma once

#include <QObject>
#include <QStringList>

#include <memory>
#include <vector>

#include "window.hpp"

namespace surfacescry {

class WindowManager : public QObject {
    Q_OBJECT

public:
    explicit WindowManager(QObject *parent = nullptr);
    ~WindowManager() override = default;

    virtual std::vector<std::shared_ptr<Window>> listWindows() const = 0;

signals:
    void windowCreated(std::shared_ptr<Window> window);
    void windowDestroyed(const QString &id);
    void windowUpdated(std::shared_ptr<Window> window, const QStringList &fields);
    void focusChanged(const QString &newId, const QString &oldId);
};

} // namespace surfacescry

Q_DECLARE_METATYPE(std::shared_ptr<surfacescry::Window>)
