/**
 * @file main.cpp
 * @brief avvia la mia applicazione widget Qt.
 */
#include "texteditor.h"

#include <QApplication>

/// Crea l'applicazione Qt e la mia finestra.
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TextEditor w;
    w.show();
    return a.exec();
}
