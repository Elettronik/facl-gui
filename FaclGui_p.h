#ifndef FACLGUI_P_H
#define FACLGUI_P_H

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtGui/QAbstractButton>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFileSystemModel>
#include <QtGui/QMessageBox>

#include "FaclGui.h"
#include "delegate/CheckBoxStyledDelegate.h"
#include "dialog/AddDialog.h"
#include "dialog/PreferencesDialog.h"
#include "model/AclTableModel.h"


class FaclGuiPrivate : public QObject{
    Q_OBJECT

public:
    FaclGuiPrivate(FaclGui* const parent);
    ~FaclGuiPrivate();
    bool checkUserSave();

public slots:
    void aclTypeSelected(int idx);
    void buttonBoxClicked(QAbstractButton *button);
    void deleteAcl();
    void loadSettings();
    void selectItemChanged(const QModelIndex idx);
    void showPreferencesDialog();
    void showAddDialog();

private:
    FaclGui* const q_ptr;
    Q_DECLARE_PUBLIC(FaclGui)

    QFileSystemModel* fileModel;
    AclTableModel* aclModel;
    CheckBoxStyledDelegate* checkDelegate;
    AddDialog* ad;
    PreferencesDialog* pd;

    //Confirm entry deletion
    bool confirmDeletion;


    //Metodi privati di utilità
    void showFileInfo(const QString &file);
    void reloadSettings();
};

#endif // FACLGUI_P_H
