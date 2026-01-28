#include <QApplication>

#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(QStringLiteral("SurfaceScry"));
    QCoreApplication::setOrganizationName(QStringLiteral("SurfaceScry"));

    surfacescry::MainWindow window;
    window.setWindowTitle(QStringLiteral("SurfaceScry"));
    window.resize(1200, 700);
    window.show();

    return app.exec();
}
