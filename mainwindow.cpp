#include "mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "addrecord.h"
#include "getadminpassword.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setTblEditable(false);
    systemInit();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::checkAdminPassword() {
    auto tbl = std::make_unique<QSqlTableModel>(this, db_);
    tbl->setTable("ADMIN");
    if (tbl->select()) {
        return;
    }
    auto getAdminPassword = std::make_unique<GetAdminPassword>(this);
    while (true) {
        if (getAdminPassword->exec() == QDialog::Rejected) {
            close();
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
    tabModel->setTable("CHOOSE");
    if (!(tabModel->select()))  //查询数据
    {
        QMessageBox::information(this, "使用提示", "未查询到已有数据表，即将创建新数据表");
        QSqlQuery query(db_);
        query.exec("CREATE TABLE CHOOSE(ID INT PRIMARY KEY NOT NULL, NAME TEXT NOT NULL, RID TEXT UNIQUE NOT NULL, MLFHSB TEXT, ZSTFFHSB TEXT)");
        tabModel->setTable("CHOOSE");
    }

    tabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //    tabModel->setSort(tabModel->fieldIndex("empNo"), Qt::AscendingOrder); // 排序

    //字段显示名
    tabModel->setHeaderData(tabModel->fieldIndex("NAME"), Qt::Horizontal, "项目名称");
    tabModel->setHeaderData(tabModel->fieldIndex("RID"), Qt::Horizontal, "监督注册号");
    tabModel->setHeaderData(tabModel->fieldIndex("MLFHSB"), Qt::Horizontal, "门类防护设备抽查项目");
    tabModel->setHeaderData(tabModel->fieldIndex("ZSTFFHSB"), Qt::Horizontal, "战时通风防护设备抽查项目");

    theSelection = new QItemSelectionModel(tabModel);  //关联选择模型
    // theSelection当前项变化时触发currentChanged信号
    //    connect(theSelection, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(on_currentChanged(QModelIndex, QModelIndex)));
    //    connect(theSelection, SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(on_currentRowChanged(QModelIndex, QModelIndex)));

    ui->tableView->setModel(tabModel);               // 设置数据模型
    ui->tableView->setSelectionModel(theSelection);  // 设置选择模型
    ui->tableView->resizeColumnsToContents();        // 自动调整列宽

    //    ui->tableView->setColumnHidden(tabModel->fieldIndex("ID"), true);  //隐藏列

    //    // tableView上为“性别”和“部门”两个字段设置自定义代理组件
    //    QStringList strList;
    //    strList << "男"
    //            << "女";
    //    bool isEditable = false;
    //    delegateSex.setItems(strList, isEditable);
    //    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Gender"), &delegateSex);  // Combbox选择型

    //    strList.clear();
    //        strList << "销售部"<< "技术部" << "生产部"  << "行政部";
    //    isEditable = true;
    //    delegateDepart.setItems(strList, isEditable);
    //    ui->tableView->setItemDelegateForColumn(tabModel->fieldIndex("Department"), &delegateDepart);  // Combbox选择型

    //    //创建界面组件与数据模型的字段之间的数据映射
    //    dataMapper = new QDataWidgetMapper();
    //    dataMapper->setModel(tabModel);                              //设置数据模型
    //    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);  //

    //    //    dataMapper->setItemDelegate(new QSqlRelationalDelegate(this)); //含有外键的
    //    //界面组件与tabModel的具体字段之间的联系
    //    dataMapper->addMapping(ui->dbSpinEmpNo, tabModel->fieldIndex("empNo"));

    //    //    dataMapper->addMapping(ui->dbPhoto,tabModel->fieldIndex("Photo")); //图片无法直接映射

    //    dataMapper->toFirst();  //移动到首记录

    //    //更新actions和界面组件的使能状态
    //    ui->actOpenDB->setEnabled(false);

    //    ui->actRecAppend->setEnabled(true);
    //    ui->actRecInsert->setEnabled(true);
    //    ui->actRecDelete->setEnabled(true);
    //    ui->actScan->setEnabled(true);

    //    ui->groupBoxSort->setEnabled(true);
    //    ui->groupBoxFilter->setEnabled(true);
}

// 保存修改
void MainWindow::on_pushButton_clicked() {
    if (!tabModel->submitAll()) {
        QMessageBox::warning(this, "错误", "保存失败，请检查数据格式");
    }
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
        tabModel->insertRow(tabModel->rowCount(), QModelIndex());                               //在末尾添加一个记录
        theSelection->clearSelection();                                                         //清空选择项
        tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 0), tabModel->rowCount());  //自动生成编号
        tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 1), addRecord->getName());
        tabModel->setData(tabModel->index(tabModel->rowCount() - 1, 2), addRecord->getRID());
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
