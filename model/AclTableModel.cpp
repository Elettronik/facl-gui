#include "AclTableModel.h"
#include "AclTableModel_p.h"

AclTableModel::AclTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    d_ptr(new AclTableModelPrivate(this))
{
}

void AclTableModel::addEntry(const acl_tag_t &type, const int &id, const bool &read, const bool &write, const bool &exec)
{
    Q_D(AclTableModel);
    qDebug() << "Acl valida: " << acl_valid(d->acl);

    //Controlliamo se c'è l'acl maschera
    bool found = false;
    int ret = 0;
    foreach(acl_entry_t* entry, d->entries)
    {
        acl_tag_t tipo;
        acl_get_tag_type(*entry, &tipo);
        if(tipo == ACL_MASK)
        {
            found = true;
        }
    }
    //Se non c'è la creo copiandola dal gruppo
    if(!found)
    {
        acl_entry_t mask_entry = NULL;
        if(acl_create_entry(&(d->acl), &mask_entry) != 0)
        {
            qCritical("Errore nel creare l'entry maschera: %s", strerror(errno));
        }
        if(acl_set_tag_type(mask_entry, ACL_MASK) != 0)
        {
            qCritical("Errore nel settare il tipo maschera: %s", strerror(errno));
        }

        //Permessi
        acl_permset_t mask_perms = NULL;
        ret = acl_get_permset(mask_entry, &mask_perms);
        if(ret != 0)
        {
            qCritical("Errore nel leggere i permessi maschera: %s", strerror(errno));
        }


        ret = acl_add_perm(mask_perms, ACL_READ);
        if(ret != 0)
        {
            qCritical("Errore nel settare la lettura maschera: %s", strerror(errno));
        }


        ret = acl_add_perm(mask_perms, ACL_WRITE);
        if(ret != 0)
        {
            qCritical("Errore nel settare la scrittura maschera: %s", strerror(errno));
        }

        ret = acl_add_perm(mask_perms, ACL_EXECUTE);
        if(ret != 0)
        {
            qCritical("Errore nel settare l'esecuzione maschera: %s", strerror(errno));
        }
    }


    acl_entry_t new_entry = NULL;

    //Creazione e tipo
    if(acl_create_entry(&(d->acl), &new_entry) != 0)
    {
        qCritical("Errore nel creare l'acl: %s", strerror(errno));
    }
    if(acl_set_tag_type(new_entry, type) != 0)
    {
        qCritical("Errore nel settare il tipo: %s", strerror(errno));
    }

    //Qualificatore (utente o gruppo) id
    uid_t uid = id;
    gid_t gid = id;
    switch(type)
    {
    case ACL_USER:
        ret = acl_set_qualifier(new_entry, &uid);
        break;
    case ACL_GROUP:
        ret = acl_set_qualifier(new_entry, &gid);
        break;
    }
    if(ret != 0)
    {
        qCritical("Errore nel settare il qualificatore: %s", strerror(errno));
    }

    //Permessi
    acl_permset_t perms = NULL;
    ret = acl_get_permset(new_entry, &perms);
    if(ret != 0)
    {
        qCritical("Errore nel leggere i permessi: %s", strerror(errno));
    }

    if(read)
    {
        ret = acl_add_perm(perms, ACL_READ);
    }
    if(ret != 0)
    {
        qCritical("Errore nel settare la lettura: %s", strerror(errno));
    }

    if(write)
    {
        ret = acl_add_perm(perms, ACL_WRITE);
    }
    if(ret != 0)
    {
        qCritical("Errore nel settare la scrittura: %s", strerror(errno));
    }

    if(exec)
    {
        ret = acl_add_perm(perms, ACL_EXECUTE);
    }
    if(ret != 0)
    {
        qCritical("Errore nel settare l'esecuzione: %s", strerror(errno));
    }

    qDebug() << "Acl valida: " << acl_valid(d->acl);

    //Applico l'acl
    acl_set_file(d->file.toLocal8Bit(), d->type, d->acl);

    reloadAcl();
}

int AclTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant AclTableModel::data(const QModelIndex &index, int role) const
{
    Q_D(const AclTableModel);

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        //Devo mostrare le acl
        switch(index.column())
        {
        case 0:
            //tipo di proprietario
            return d->getEntry(index.row(), AclTableModelPrivate::ENTRY_OBJECT);
        case 1:
            //nome del proprietario
            return d->getEntry(index.row(), AclTableModelPrivate::ENTRY_NAME);
        case 2:
            return d->getEntry(index.row(), AclTableModelPrivate::ENTRY_READ);
        case 3:
            return d->getEntry(index.row(), AclTableModelPrivate::ENTRY_WRITE);
        case 4:
            return d->getEntry(index.row(), AclTableModelPrivate::ENTRY_EXEC);
        default:
            return "Error";
        }
    }else{
        return QVariant();
    }

}

void AclTableModel::deleteEntry(const int &idx)
{
    if(isDeletable(idx))
    {
        Q_D(AclTableModel);
        if(acl_delete_entry(d->acl, *(d->entries.at(idx))) != 0)
        {
            qCritical("Errore nel cancellare l'entry: %s", strerror(errno));
        }

        //Applico l'acl
        acl_set_file(d->file.toLocal8Bit(), d->type, d->acl);

        reloadAcl();
    }
}

Qt::ItemFlags AclTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
    {
        return QAbstractTableModel::flags(index);
    }

    if(index.column() >= 2 && index.column() <= 4)
    {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }else{
        return QAbstractTableModel::flags(index);
    }
}


QVariant AclTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if(orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case 0:
            return tr("Type");
        case 1:
            return tr("Name");
        case 2:
            return tr("Read");
        case 3:
            return tr("Write");
        case 4:
            return tr("Exec");
        }
    }

    return QVariant();
}

bool AclTableModel::isDeletable(const int &idx)
{
    Q_D(AclTableModel);
    acl_entry_t* entry = d->entries.at(idx);
    acl_tag_t tag;
    acl_get_tag_type(*entry, &tag);

    switch(tag)
    {
    case ACL_USER:
    case ACL_GROUP:
        return true;
    case ACL_MASK:
        return d->isOnlyMask();
    default:
        return false;
    }
}

bool AclTableModel::isModified()
{
    Q_D(AclTableModel);
    return d->modified;
}

void AclTableModel::reloadAcl()
{
    Q_D(AclTableModel);
    setFileName(d->file);
}

int AclTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    Q_D(const AclTableModel);
    return acl_entries(d->acl);
}

void AclTableModel::saveAcl()
{
    Q_D(AclTableModel);
    if(acl_set_file(d->file.toLocal8Bit(), d->type, d->acl) == -1)
    {
        qDebug() << strerror(errno);
    }

    d->modified = false;
}

void AclTableModel::setAclType(acl_type_t type)
{
    Q_D(AclTableModel);
    d->type = type;
    reloadAcl();
}

bool AclTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(AclTableModel);

    if(index.isValid() && role == Qt::EditRole)
    {
        if(index.column() >= 2 || index.column() <=4)
        {
            switch(index.column())
            {
            case 2:
                return d->setPerm(d->entries.at(index.row()), ACL_READ, value.toBool());
            case 3:
                return d->setPerm(d->entries.at(index.row()), ACL_WRITE, value.toBool());
            case 4:
                return d->setPerm(d->entries.at(index.row()), ACL_EXECUTE, value.toBool());
            }
        }
    }

    return false;
}

void AclTableModel::setFileName(const QString file)
{
    Q_D(AclTableModel);
    acl_t acl = NULL;
    acl = acl_get_file(file.toLocal8Bit(), d->type);
    d->setAcl(acl);

    d->file = file;
    d->modified = false;

    qDebug() << file << "\n" << acl_to_text(d->acl, NULL);
}

AclTableModelPrivate::AclTableModelPrivate(AclTableModel *parent) :
    QObject(parent),
    q_ptr(parent),
    type(ACL_TYPE_ACCESS),
    modified(false)
{
}

QVariant AclTableModelPrivate::getEntry(int number, const EntryType &type) const
{
    switch(type)
    {
    case ENTRY_OBJECT:
        return getTagType(entries.at(number));
    case ENTRY_NAME:
        return getTagName(entries.at(number));
    case ENTRY_READ:
        return getPerm(entries.at(number), ACL_READ);
    case ENTRY_WRITE:
        return getPerm(entries.at(number), ACL_WRITE);
    case ENTRY_EXEC:
        return getPerm(entries.at(number), ACL_EXECUTE);
    default:
        return "";
    }
}

inline QString AclTableModelPrivate::getTagType(acl_entry_t* const entry) const
{
    acl_tag_t type;

    if(acl_get_tag_type(*entry, &type) == -1)
        return tr("Error");

    switch(type)
    {
    case ACL_USER_OBJ:
        return tr("Owner");
    case ACL_USER:
        return tr("User");
    case ACL_GROUP_OBJ:
        return tr("Owner group");
    case ACL_GROUP:
        return tr("Group");
    case ACL_OTHER:
        return tr("Other");
    case ACL_MASK:
        return tr("Mask");
    default:
        return tr("Error");
    }
}

inline QString AclTableModelPrivate::getTagName(acl_entry_t* const entry) const
{
    acl_tag_t type;
    if(acl_get_tag_type(*entry, &type) == -1)
        return tr("Error");

    switch(type)
    {
    case ACL_USER:
        return userQualifier(entry);
    case ACL_GROUP:
        return groupQualifier(entry);
    default:
        return "";
    }
}

inline QVariant AclTableModelPrivate::getPerm(acl_entry_t* const entry, const acl_perm_t &perm) const
{
    acl_permset_t perm_set;
    acl_get_permset(*entry, &perm_set);

    if(acl_get_perm(perm_set, perm) == 1)
    {
        return QVariant(true);
    }else{
        return QVariant(false);
    }
}

inline bool AclTableModelPrivate::isOnlyMask()
{
    bool only = true;
    acl_tag_t tag;
    foreach (acl_entry_t* entry, entries)
    {
        acl_get_tag_type(*entry, &tag);
        if( tag == ACL_USER || tag == ACL_GROUP)
        {
            only = false;
        }
    }

    return only;
}

void AclTableModelPrivate::setAcl(const acl_t acl)
{
    Q_Q(AclTableModel);
    acl_entry_t* entry;

    q->beginResetModel();
    this->acl = acl;

    //Pulisco le vecchie entry
    foreach(entry, entries)
    {
        delete entry;
    }
    entries.clear();

    //Leggo le nuove
    entry = new acl_entry_t;
    acl_get_entry(acl, ACL_FIRST_ENTRY, entry);
    entries.append(entry);

    entry = new acl_entry_t;
    while(acl_get_entry(acl, ACL_NEXT_ENTRY, entry) == 1)
    {
        entries.append(entry);
        entry = new acl_entry_t;
    }
    delete entry;

    q->endResetModel();
}

inline bool AclTableModelPrivate::setPerm(acl_entry_t* const entry, const acl_perm_t &perm, const bool &value)
{
    acl_permset_t permission;

    acl_get_permset(*entry, &permission);

    if(value)
    {
        //Aggiungere un permesso
        acl_add_perm(permission, perm);
    }else{
        //Rimuovo permesso
        acl_delete_perm(permission, perm);
    }

    modified = true;

    return true;
}

inline QString AclTableModelPrivate::userQualifier(const acl_entry_t* const entry) const
{
    uid_t* user = (uid_t*)acl_get_qualifier(*entry);
    if(user)
    {
        struct passwd* pwname = getpwuid(*user);
        acl_free((void *)user);
        return QString(pwname->pw_name);
    }else{
        return QString();
    }
}

inline QString AclTableModelPrivate::groupQualifier(const acl_entry_t* const entry) const
{
    gid_t* group = (gid_t*)acl_get_qualifier(*entry);
    if(group)
    {
        struct group* grname = getgrgid(*group);
        acl_free((void *)group);
        return QString(grname->gr_name);
    }else{
        return QString();
    }
}
