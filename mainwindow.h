#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "qwcomboboxdelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    void systemInit();
    void setTblEditable(bool isEditable);
    void checkAdminPassword();
    QSqlDatabase db_;
    QSqlTableModel *tabModel;
    QItemSelectionModel *theSelection;
    QWidgetMapper *dataMapper;
    QWComboBoxDelegate comboDeligate1;
    QWComboBoxDelegate comboDeligate2;
};
#endif  // MAINWINDOW_H
