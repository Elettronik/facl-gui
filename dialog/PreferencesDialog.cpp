#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    QSettings settings;

    //Settings Linguaggio
    settings.beginGroup("Language");
    ui->comboBoxLanguage->setCurrentIndex(settings.value("lang_idx", QVariant(0)).toInt());
    settings.endGroup();

    //Setting User e gruppi
    settings.beginGroup("UserAndGroups");
    ui->checkBoxSystemUsers->setChecked(settings.value("users").toBool());
    ui->checkBoxSystemGroups->setChecked(settings.value("groups").toBool());
    settings.endGroup();

    //Setting deletion
    settings.beginGroup("Delete");
    ui->checkBoxConfirmDelete->setChecked(settings.value("confirmDeletion").toBool());
    settings.endGroup();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::changeEvent(QEvent *e)
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

void PreferencesDialog::saveSettings()
{
    QSettings settings;
    //Settings Linguaggio
    settings.beginGroup("Language");
    settings.setValue("lang_idx", ui->comboBoxLanguage->currentIndex());
    settings.endGroup();

    //Settings Users and Groups
    settings.beginGroup("UserAndGroups");
    settings.setValue("users", ui->checkBoxSystemUsers->isChecked());
    settings.setValue("groups", ui->checkBoxSystemGroups->isChecked());
    settings.endGroup();

    //Setting Deletion
    settings.beginGroup("Delete");
    settings.setValue("confirmDeletion", ui->checkBoxConfirmDelete->isChecked());
    settings.endGroup();
    settings.sync();
}
