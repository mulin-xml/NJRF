#include "addrecord.h"

#include "ui_addrecord.h"

AddRecord::AddRecord(QWidget *parent) : QDialog(parent), ui(new Ui::AddRecord) { ui->setupUi(this); }

AddRecord::~AddRecord() { delete ui; }

QString AddRecord::getName() { return ui->lineEdit->text(); }

QString AddRecord::getRID() { return ui->lineEdit_2->text(); }

bool AddRecord::isMLFHSBChecked() { return ui->checkBox->isChecked(); }

bool AddRecord::isZSTFFHSBChecked() { return ui->checkBox_2->isChecked(); }
