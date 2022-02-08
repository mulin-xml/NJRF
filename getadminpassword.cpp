#include "getadminpassword.h"

#include "ui_getadminpassword.h"

GetAdminPassword::GetAdminPassword(QWidget *parent) : QDialog(parent), ui(new Ui::GetAdminPassword) {
    ui->setupUi(this);
    setFixedSize(size());
}

GetAdminPassword::~GetAdminPassword() { delete ui; }

QString GetAdminPassword::getPassword() {
    if (ui->lineEdit->text() == ui->lineEdit_2->text()) {
        return ui->lineEdit->text();
    }
    return "";
}
