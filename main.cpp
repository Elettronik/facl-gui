#include <QtCore/QSettings>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

#include "FaclGui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;


    a.setOrganizationName("Elettronik");
    a.setApplicationName("Facl Gui");

    QSettings settings;
    int lang_idx = settings.value("Language/lang_idx", QVariant(0)).toInt();
    switch(lang_idx)
    {
    case 1:
        translator.load(":/translation/it/facl-guit_it.qm");
        break;
    default:
        break;
    }

    a.installTranslator(&translator);

    FaclGui w;
    w.show();

    return a.exec();
}
