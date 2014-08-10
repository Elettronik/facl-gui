#ifndef ADDDIALOG_P_H
#define ADDDIALOG_P_H

#include <grp.h>
#include <pwd.h>
#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QSettings>
#include "AddDialog.h"

class AddDialogPrivate : public QObject
{
    Q_OBJECT
public:
    AddDialogPrivate(AddDialog* parent);
    virtual ~AddDialogPrivate();
    void load_settings();

public slots:
    void typeChanged(int idx);

private:
    AddDialog * const q_ptr;
    Q_DECLARE_PUBLIC(AddDialog)
    QMap<int, QString> names;
    //Settings
    bool showSystemUsers;
    bool showSystemGroups;

    void fillGroups();
    void fillUsers();
};

#endif // ADDDIALOG_P_H
