#ifndef ADDRECORD_H
#define ADDRECORD_H

#include <QDialog>

namespace Ui {
class AddRecord;
}

class AddRecord : public QDialog {
    Q_OBJECT

   public:
    explicit AddRecord(QWidget *parent = nullptr);
    ~AddRecord();
    QString getName();
    QString getRID();
    bool isMLFHSBChecked();
    bool isZSTFFHSBChecked();

   private:
    Ui::AddRecord *ui;
};

#endif  // ADDRECORD_H
