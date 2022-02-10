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
    int getRd();
    void saveTbl();
    QSqlDatabase db_;
    QSqlTableModel *tabModel;
    QItemSelectionModel *theSelection;
    QWComboBoxDelegate comboDeligate1_;
    QWComboBoxDelegate comboDeligate2_;
    const QStringList MLFHSB_{"钢筋混凝土门", "钢结构门", "悬摆式防爆波活门", "无"};
    const QStringList ZSTFFHSB_{"密闭阀门", "超压排气活门", "油网滤尘器", "过滤吸收器", "无"};
};
#endif  // MAINWINDOW_H
