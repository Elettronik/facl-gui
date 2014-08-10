#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtCore/QSettings>
#include <QtGui/QDialog>

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();
    void saveSettings();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
