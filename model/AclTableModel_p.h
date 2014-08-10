#ifndef ACLTABLEMODEL_P_H
#define ACLTABLEMODEL_P_H

#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <acl/libacl.h>
#include <QtCore/QSettings>
#include <QtCore/QVariant>

#include "AclTableModel.h"


class AclTableModelPrivate : public QObject
{
    Q_OBJECT
public:
    //Enumerazioni
    enum EntryType { ENTRY_OBJECT, ENTRY_NAME, ENTRY_READ, ENTRY_WRITE, ENTRY_EXEC };

    explicit AclTableModelPrivate(AclTableModel *parent);
    void setAcl(const acl_t acl);
    QVariant getEntry(int number, const EntryType &type ) const;
    inline QVariant getPerm(acl_entry_t* const entry, const acl_perm_t &perm) const;
    inline QString getTagName(acl_entry_t* const entry) const;
    inline QString getTagType(acl_entry_t* const entry) const;
    inline bool isOnlyMask();
    inline bool setPerm(acl_entry_t* const entry, const acl_perm_t &perm, const bool &value);

signals:

public slots:

private:
    AclTableModel * const q_ptr;
    Q_DECLARE_PUBLIC(AclTableModel)

    acl_t acl;
    acl_type_t type;
    QList<acl_entry_t *> entries;
    QString file;
    bool modified;

    //Metodi privati
    inline QString userQualifier(const acl_entry_t* const entry) const;
    inline QString groupQualifier(const acl_entry_t* const entry) const;
};

#endif // ACLTABLEMODEL_P_H
