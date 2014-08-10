#ifndef ACLTABLEMODEL_H
#define ACLTABLEMODEL_H

#include <QtCore/QDebug>
#include <QtCore/QAbstractTableModel>
#include <sys/acl.h>


class AclTableModelPrivate;

class AclTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AclTableModel(QObject *parent = 0);

    //Metodi implementati dalla superclasse
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    //Metodi dell'implementazione
    void addEntry(const acl_tag_t &type, const int &id, const bool &read, const bool &write, const bool &exec);
    void deleteEntry(const int &idx);
    bool isDeletable(const int &idx);
    bool isModified();
    void setAclType(acl_type_t type);
    void setFileName(const QString file);
signals:

public slots:
    void reloadAcl();
    void saveAcl();

private:
    AclTableModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(AclTableModel)

};

#endif // ACLTABLEMODEL_H
