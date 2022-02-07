#include "mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    on_pushButton_clicked();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::openTable() {
    tabModel = new QSqlTableModel(this, db_);
    tabModel->setTable("CHOOSE");
    if (!(tabModel->select()))  //查询数据
    {
        QMessageBox::information(this, "使用提示", "未查询到已有数据表，即将创建新数据表");
        QSqlQuery query(db_);
        query.exec("CREATE TABLE CHOOSE(ID INT PRIMARY KEY NOT NULL, NAME TEXT NOT NULL, RID TEXT NOT NULL, MLFHSB TEXT, ZSTFFHSB TEXT)");
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

    ui->tableView->setModel(tabModel);               //设置数据模型
    ui->tableView->setSelectionModel(theSelection);  //设置选择模型
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

void MainWindow::on_pushButton_clicked() {
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName("dataset.db");
    if (!db_.open()) {
        QMessageBox::warning(this, "错误", "打开数据库失败");
        close();
    }
    auto tbl = new QSqlTableModel(this, db_);
    tbl->setTable("ADMIN");
    tbl->setEditStrategy(QSqlTableModel::OnManualSubmit);
    if (!(tbl->select()))  //查询数据
    {
        QMessageBox::information(this, "使用提示", "首次使用请设置管理员密码");
        QSqlQuery query(db_);
        query.exec("CREATE TABLE ADMIN(PASSWORD TEXT)");
    }
    openTable();
}

//添加记录
void MainWindow::on_pushButton_2_clicked() {
    tabModel->insertRow(tabModel->rowCount(), QModelIndex());              //在末尾添加一个记录
    QModelIndex curIndex = tabModel->index(tabModel->rowCount() - 1, 1);   //创建最后一行的ModelIndex
    theSelection->clearSelection();                                        //清空选择项
    theSelection->setCurrentIndex(curIndex, QItemSelectionModel::Select);  //设置刚插入的行为当前选择行
//    int currow = curIndex.row();                                           //获得当前行
//    tabModel->setData(tabModel->index(currow, 0), tabModel->rowCount());   //自动生成编号
//    tabModel->setData(tabModel->index(currow, 1), "默认项目名称");
//    tabModel->setData(tabModel->index(currow, 2), "0000-0000");

//    if (!tabModel->submitAll()) {
//        QMessageBox::information(this, "消息", "数据保存错误");
//    }
}
