#include "qwcomboboxdelegate.h"

#include <QComboBox>

QWComboBoxDelegate::QWComboBoxDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void QWComboBoxDelegate::setItems(QStringList items) { m_ItemList = items; }

// 用于创建需要的编辑组件
QWidget *QWComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox *editor = new QComboBox(parent);
    for (int i = 0; i < m_ItemList.count(); i++)  //从字符串列表初始下拉列表
        editor->addItem(m_ItemList.at(i));

    return editor;
}

// 用于从数据模型获取数值，设置为编辑器的显示值，当双击单元格进入编辑状态时就会自动调用此函数
void QWComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    comboBox->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
}

// 用于将代理编辑器上的值更新给数据模型，当用户在界面上完成编辑时会自动调用次函数，将界面上的数据更新到数据模型
void QWComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox *comboBox = static_cast<QComboBox *>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// 为代理组件设置一个合适的大小，函数传递的参数option的rect变量定义了单元格适合显示代理组件的大小
void QWComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
