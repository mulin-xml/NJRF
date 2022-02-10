#ifndef GETADMINPASSWORD_H
#define GETADMINPASSWORD_H

#include <QDialog>

namespace Ui {
class GetAdminPassword;
}

class GetAdminPassword : public QDialog {
    Q_OBJECT

   public:
    explicit GetAdminPassword(QWidget *parent = nullptr);
    ~GetAdminPassword();
    QString getPassword();

   private:
    Ui::GetAdminPassword *ui;
};

#endif  // GETADMINPASSWORD_H
