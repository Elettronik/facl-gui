#include "FaclGui.h"
#include "FaclGui_p.h"
#include "ui_FaclGui.h"

FaclGui::FaclGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FaclGui),
    d_ptr(new FaclGuiPrivate(this))
{
    Q_D(FaclGui);

    //Splitter Options
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,1);
    ui->splitter->setStretchFactor(2,2);

    //Tree view Options
    ui->treeViewFiles->setModel(d->fileModel);
    ui->treeViewFiles->setColumnHidden(1, true);
    ui->treeViewFiles->setColumnHidden(2, true);
    ui->treeViewFiles->setColumnHidden(3, true);
    ui->treeViewFiles->setRootIndex(d->fileModel->index(QDir::homePath()));

    //AclView Options
    ui->tableViewAcl->setModel(d->aclModel);
    ui->tableViewAcl->setItemDelegateForColumn(2, d->checkDelegate);
    ui->tableViewAcl->setItemDelegateForColumn(3, d->checkDelegate);
    ui->tableViewAcl->setItemDelegateForColumn(4, d->checkDelegate);

    //Action Options
    ui->action_Preferences->setIcon(QIcon::fromTheme("document-properties"));
    ui->action_Reset->setIcon(QIcon::fromTheme("document-revert"));
    ui->action_Save->setIcon(QIcon::fromTheme("document-save"));

    //signal & slots
    connect(ui->treeViewFiles->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            d, SLOT(selectItemChanged(QModelIndex)));
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
            d, SLOT(buttonBoxClicked(QAbstractButton*)));
    connect(ui->action_Quit, SIGNAL(triggered()),
            QApplication::instance(), SLOT(quit()));
    connect(ui->action_Reset, SIGNAL(triggered()),
            d->aclModel, SLOT(reloadAcl()));
    connect(ui->action_Save, SIGNAL(triggered()),
            d->aclModel, SLOT(saveAcl()));
    connect(ui->action_Preferences, SIGNAL(triggered()),
            d, SLOT(showPreferencesDialog()));
    connect(ui->comboBoxSelectAcl, SIGNAL(currentIndexChanged(int)),
            d, SLOT(aclTypeSelected(int)));
    connect(ui->pushButtonAdd, SIGNAL(clicked()),
            d, SLOT(showAddDialog()));
    connect(ui->pushButtonDelete, SIGNAL(clicked()),
            d, SLOT(deleteAcl()));
}

FaclGui::~FaclGui()
{
    delete ui;
}

void FaclGui::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FaclGui::closeEvent(QCloseEvent *event)
{
    Q_D(FaclGui);
    if(d->checkUserSave())
    {
        event->accept();
    }else{
        event->ignore();
    }
}

FaclGuiPrivate::FaclGuiPrivate(FaclGui *const parent) :
    q_ptr(parent),
    fileModel(new QFileSystemModel(this)),
    aclModel(new AclTableModel(this)),
    checkDelegate(new CheckBoxStyledDelegate(this)),
    ad(NULL),
    pd(NULL)
{
    fileModel->setRootPath("/");

    loadSettings();
}

FaclGuiPrivate::~FaclGuiPrivate()
{
    delete aclModel;
    delete fileModel;
    delete checkDelegate;
    if(pd != NULL)
    {
        delete pd;
    }
    if(ad != NULL)
    {
        delete ad;
    }
}

void FaclGuiPrivate::aclTypeSelected(int idx)
{
    switch(idx)
    {
    case 0:
        aclModel->setAclType(ACL_TYPE_ACCESS);
        break;
    case 1:
        aclModel->setAclType(ACL_TYPE_DEFAULT);
        break;
    }
}

void FaclGuiPrivate::buttonBoxClicked(QAbstractButton *button)
{
    Q_Q(FaclGui);

    QDialogButtonBox::ButtonRole role = q->ui->buttonBox->buttonRole(button);

    switch(role)
    {
    case QDialogButtonBox::AcceptRole:
        aclModel->saveAcl();
        break;
    case QDialogButtonBox::ResetRole:
        aclModel->reloadAcl();
        break;
    default:
        break;
    }
}

bool FaclGuiPrivate::checkUserSave()
{
    Q_Q(FaclGui);
    if(aclModel->isModified())
    {
        QMessageBox::StandardButton ret = QMessageBox::question( q,
                                                                 tr("Acl not saved"),
                                                                 tr("An acl is not saved, wanna save it?"),
                                                                 QMessageBox::Cancel | QMessageBox::Save | QMessageBox::Discard,
                                                                 QMessageBox::Cancel);
        switch(ret)
        {
        case QMessageBox::Cancel:
            return false;
        case QMessageBox::Save:
            aclModel->saveAcl();
        case QMessageBox::Discard:
            aclModel->reloadAcl();
        default:
            break;
        }
    }
    return true;
}

void FaclGuiPrivate::deleteAcl()
{
    Q_Q(FaclGui);
    QItemSelectionModel* s_model = q->ui->tableViewAcl->selectionModel();
    QModelIndex s_index = s_model->currentIndex();
    if(s_index.isValid())
    {
        int idx = s_index.row();
        if(aclModel->isDeletable(idx))
        {
            if(confirmDeletion)
            {
                QMessageBox::StandardButton ret = QMessageBox::question( q,
                                                                         tr("Confirm deletion"),
                                                                         tr("Really wanna delete this entry?"),
                                                                         QMessageBox::Cancel | QMessageBox::Ok,
                                                                         QMessageBox::Cancel);
                if(ret == QMessageBox::Cancel)
                {
                    return;
                }
            }
            aclModel->deleteEntry(idx);
        }
    }
}

void FaclGuiPrivate::loadSettings()
{
    QSettings settings;
    settings.beginGroup("Delete");
    confirmDeletion = settings.value("confirmDeletion").toBool();
    settings.endGroup();
}

void FaclGuiPrivate::reloadSettings()
{
    if(ad){
        ad->reloadSettings();
    }

    loadSettings();
}

void FaclGuiPrivate::selectItemChanged(const QModelIndex idx)
{
    Q_Q(FaclGui);
    if(checkUserSave())
    {
        QString path = fileModel->filePath(idx);
        aclModel->setAclType(ACL_TYPE_ACCESS);
        aclModel->setFileName(path);
        showFileInfo(path);
        if(QFileInfo(path).isDir())
        {
            q->ui->widgetSelectAcl->setVisible(true);
        }else{
            q->ui->widgetSelectAcl->setVisible(false);
        }
        q->ui->comboBoxSelectAcl->setCurrentIndex(0);
    }
}

void FaclGuiPrivate::showAddDialog()
{
    Q_Q(FaclGui);
    if(ad == NULL)
    {
        ad = new AddDialog(q);
    }

    int ret = ad->exec();
    if(ret == QDialog::Accepted)
    {
        aclModel->addEntry(ad->getType(),
                           ad->getId(),
                           ad->getRead(),
                           ad->getWrite(),
                           ad->getExec());
    }
}

void FaclGuiPrivate::showFileInfo(const QString &file)
{
    Q_Q(FaclGui);
    QFileInfo finfo(file);

    q->ui->lineEditName->setText(finfo.fileName());
    q->ui->lineEditSize->setText(QString::number(finfo.size()));
    q->ui->lineEditCreation->setText(finfo.created().toString(Qt::SystemLocaleShortDate));
    q->ui->lineEditModification->setText(finfo.lastModified().toString(Qt::SystemLocaleShortDate));
}

void FaclGuiPrivate::showPreferencesDialog()
{
    Q_Q(FaclGui);
    if(pd == NULL)
    {
        pd = new PreferencesDialog(q);
    }

    if(pd->exec() == QDialog::Accepted)
    {
        pd->saveSettings();
        reloadSettings();
    }
}
