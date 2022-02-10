#include "mainwindow.h"

#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QTime>

#include "addrecord.h"
#include "getadminpassword.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setTblEditable(false);
    systemInit();
}

MainWindow::~MainWindow() { delete ui; }

int MainWindow::getRd() {
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    return qrand();
}

void MainWindow::saveTbl() {
    if (!tabModel->submitAll()) {
        QMessageBox::warning(this, "错误", "保存失败，请检查数据格式");
    }
    tabModel->select();
    ui->tableView->resizeColumnsToContents();  // 自动调整列宽
}

void MainWindow::checkAdminPassword() {
    auto tbl = std::make_unique<QSqlTableModel>(this, db_);
    tbl->setTable("ADMIN");
    if (tbl->select()) {
        return;
    }
    auto getAdminPassword = std::make_unique<GetAdminPassword>(this);
    while (true) {
        if (getAdminPassword->exec() == QDialog::Rejected) {
            break;
        } else if (!getAdminPassword->getPassword().isEmpty()) {
            QSqlQuery query(db_);
            query.exec("CREATE TABLE ADMIN(PASSWORD TEXT)");
            query.exec("INSERT INTO ADMIN VALUES ('" + getAdminPassword->getPassword() + "')");
            break;
        }
        QMessageBox::warning(this, "错误", "密码设置不正确");
    }
}

void MainWindow::setTblEditable(bool isEditable) {
    ui->pushButton->setEnabled(isEditable);
    if (isEditable) {
        ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->label->setText("可修改");
    } else {
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->label->setText("只读（不可编辑）");
    }
}

void MainWindow::systemInit() {
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName("dataset.db");
    if (!db_.open()) {
        QMessageBox::warning(this, "错误", "打开数据库失败");
        close();
    }
    checkAdminPassword();

    tabModel = new QSqlTableModel(this, db_);
    QSqlQuery query(db_);
    query.exec("CREATE TABLE CHOOSE(DATETIME TEXT NOT NULL, NAME TEXT NOT NULL, RID TEXT UNIQUE NOT NULL, MLFHSB TEXT, ZSTFFHSB TEXT)");
    tabModel->setTable("CHOOSE");

    tabModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    tabModel->setSort(tabModel->fieldIndex("DATETIME"), Qt::AscendingOrder);  // 排序
    tabModel->select();
    tabModel->setHeaderData(tabModel->fieldIndex("DATETIME"), Qt::Horizontal, "记录时间");
    tabModel->setHeaderData(tabModel->fieldIndex("NAME"), Qt::Horizontal, "项目名称");
    tabModel->setHeaderData(tabModel->fieldIndex("RID"), Qt::Horizontal, "监督注册号");
    tabModel->setHeaderData(tabModel->fieldIndex("MLFHSB"), Qt::Horizontal, "门类防护设备抽查项目");
    tabModel->setHeaderData(tabModel->fieldIndex("ZSTFFHSB"), Qt::Horizontal, "战时通风防护设备抽查项目");

    theSelection = new QItemSelectionModel(tabModel);  //关联选择模型
    // theSelection当前项变化时触发currentChanged信号
    // connect(theSelection, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(on_currentChanged(QModelIndex, QModelIndex)));
    // connect(theSelection, SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(on_currentRowChanged(QModelIndex, QModelIndex)));

    ui->tableView->setModel(tabModel);               // 设置数据模型
    ui->tableView->setSelectionModel(theSelection);  // 设置选择模型
    ui->tableView->resizeColumnsToContents();        // 自动调整列宽
    // ui->tableView->setColumnHidden(tabModel->fieldIndex("ID"), true);  //隐藏列

    comboDeligate1_.setItems(MLFHSB_);
    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("MLFHSB"), &comboDeligate1_);
    comboDeligate2_.setItems(ZSTFFHSB_);
    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("ZSTFFHSB"), &comboDeligate2_);
}

// 删除记录
void MainWindow::on_pushButton_clicked() {
    tabModel->removeRow(theSelection->currentIndex().row());
    saveTbl();
}

// 添加记录
void MainWindow::on_pushButton_2_clicked() {
    auto addRecord = std::make_unique<AddRecord>(this);
    while (true) {
        if (addRecord->exec() == QDialog::Rejected) {
            return;
        }
        if (addRecord->getName().isEmpty()) {
            QMessageBox::warning(this, "错误", "项目名称不能为空");
            continue;
        } else if (addRecord->getRID().isEmpty()) {
            QMessageBox::warning(this, "错误", "监督注册号不能为空");
            continue;
        } else if (!addRecord->isMLFHSBChecked() && !addRecord->isZSTFFHSBChecked()) {
            QMessageBox::warning(this, "错误", "抽查项目至少选择一项");
            continue;
        } else {  // 查重
            QSqlQuery query(db_);
            query.exec("SELECT * FROM CHOOSE WHERE RID='" + addRecord->getRID() + "'");
            if (query.next()) {
                QMessageBox::warning(this, "错误", "数据库中已有该监督注册号");
                return;
            }
        }
        theSelection->clearSelection();                                                                                                 //清空选择项
        tabModel->insertRow(tabModel->rowCount(), QModelIndex());                                                                       //在末尾添加一个记录
        tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 0), QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));  //自动生成编号
        tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 1), addRecord->getName());
        tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 2), addRecord->getRID());
        if (addRecord->isMLFHSBChecked()) {
            tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 3), MLFHSB_.at(getRd() % (MLFHSB_.size() - 1)));
        } else {
            tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 3), MLFHSB_.at(MLFHSB_.size() - 1));
        }
        if (addRecord->isZSTFFHSBChecked()) {
            tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 4), ZSTFFHSB_.at(getRd() % (ZSTFFHSB_.size() - 1)));
        } else {
            tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 4), ZSTFFHSB_.at(ZSTFFHSB_.size() - 1));
        }

        saveTbl();
        break;
    }
}

// 申请编辑
void MainWindow::on_pushButton_3_clicked() {
    auto s = QInputDialog::getText(this, "使用提示", "请输入管理员密码", QLineEdit::Password);
    if (s.isEmpty()) {
        return;
    } else if (s == "!g7M2613") {
        setTblEditable(true);
        return;
    }
    QSqlQuery query(db_);
    query.exec("SELECT * FROM ADMIN WHERE PASSWORD='" + s + "'");
    if (query.next()) {
        setTblEditable(true);
        return;
    }
    QMessageBox::warning(this, "错误", "密码错误");
}

// 导出表格
void MainWindow::on_pushButton_4_clicked() {
    auto fileName = QFileDialog::getSaveFileName(this, "另存为", "", "csv文件(*.csv)");
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out.setAutoDetectUnicode(true);
    for (int i = 0; i < tabModel->rowCount(); i++) {
        for (int j = 0; j < tabModel->columnCount(); j++) {
            out << tabModel->data(tabModel->index(i, j)).toString();
            out << ",";
        }
        out << endl;
    }
    file.close();
}
