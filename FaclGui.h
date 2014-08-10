#ifndef FACLGUI_H
#define FACLGUI_H

#include <QtCore/QDebug>
#include <QtGui/QMainWindow>

namespace Ui {
    class FaclGui;
}

class FaclGuiPrivate;

class FaclGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit FaclGui(QWidget *parent = 0);
    ~FaclGui();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::FaclGui *ui;
    FaclGuiPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(FaclGui)
};

#endif // FACLGUI_H
