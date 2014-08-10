#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QtGui/QDialog>
#include <acl/libacl.h>

namespace Ui {
    class AddDialog;
}

class AddDialogPrivate;

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDialog(QWidget *parent = 0);
    ~AddDialog();

    bool getExec();
    int getId();
    bool getRead();
    acl_tag_t getType();
    bool getWrite();
    void reloadSettings();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AddDialog *ui;
    AddDialogPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(AddDialog)
};

#endif // ADDDIALOG_H
