#include "AddDialog.h"
#include "AddDialog_p.h"
#include "ui_AddDialog.h"

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog),
    d_ptr(new AddDialogPrivate(this))
{
    Q_D(AddDialog);
    ui->setupUi(this);

    d->fillUsers();

    connect(ui->comboBoxType, SIGNAL(currentIndexChanged(int)),
            d, SLOT(typeChanged(int)));
}

AddDialog::~AddDialog()
{
    delete d_ptr;
    delete ui;
}

void AddDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool AddDialog::getExec()
{
    return ui->checkBoxExec->isChecked();
}

int AddDialog::getId()
{
    Q_D(AddDialog);
    return d->names.key(ui->comboBoxName->currentText());
}

bool AddDialog::getRead()
{
    return ui->checkBoxRead->isChecked();
}

acl_tag_t AddDialog::getType()
{
    switch(ui->comboBoxType->currentIndex())
    {
    case 0:
        return ACL_USER;
    default:
        return ACL_GROUP;
    }
}

bool AddDialog::getWrite()
{
    return ui->checkBoxWrite->isChecked();
}

void AddDialog::reloadSettings()
{
    Q_D(AddDialog);
    d->load_settings();
    d->typeChanged(ui->comboBoxType->currentIndex());
}

AddDialogPrivate::AddDialogPrivate(AddDialog* parent) :
    QObject(parent),
    q_ptr(parent)
{
    load_settings();
}

AddDialogPrivate::~AddDialogPrivate()
{
}

void AddDialogPrivate::fillGroups()
{
    Q_Q(AddDialog);
    group* groups;

    //Svuoto la mappa
    names.clear();
    Q_CHECK_PTR(q);
    q->ui->comboBoxName->clear();

    //Leggo i nuovi utenti
    setgrent();
    while((groups = getgrent()) != NULL)
    {
        //E' duplicato come ottimizzazione per evitare troppi check
        if(showSystemGroups)
        {
            names.insert(groups->gr_gid, groups->gr_name);
        }else{
            if(groups->gr_gid >= 1000)
            {
                names.insert(groups->gr_gid, groups->gr_name);
            }
        }
    }
    endgrent();
    //Mostro gli utenti
    q->ui->comboBoxName->insertItems(0, names.values());
}

void AddDialogPrivate::fillUsers()
{
    Q_Q(AddDialog);
    passwd* user;

    //Svuoto la mappa
    names.clear();
    Q_CHECK_PTR(q);
    q->ui->comboBoxName->clear();

    //Leggo i nuovi utenti
    setpwent();
    while((user = getpwent()) != NULL)
    {
        if(showSystemUsers)
        {
            names.insert(user->pw_uid, user->pw_name);
        }else{
            if(user->pw_uid >= 1000)
            {
                names.insert(user->pw_uid, user->pw_name);
            }
        }
    }
    endpwent();

    //Mostro gli utenti
    q->ui->comboBoxName->insertItems(0, names.values());
}

void AddDialogPrivate::load_settings()
{
    QSettings settings;
    settings.beginGroup("UserAndGroups");
    showSystemUsers = settings.value("users", QVariant(false)).toBool();
    showSystemGroups = settings.value("groups", QVariant(false)).toBool();
    settings.endGroup();
}

void AddDialogPrivate::typeChanged(int idx)
{
    switch(idx)
    {
    case 0:
        fillUsers();
        break;
    case 1:
        fillGroups();
        break;
    }
}
