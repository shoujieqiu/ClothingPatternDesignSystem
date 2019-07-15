#include <QInputDialog>
#include "dialogaggignparameter.h"
#include "ui_dialogaggignparameter.h"

DialogAggignParameter::DialogAggignParameter(QString arg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAggignParameter)
{
    name = arg;

    ui->setupUi(this);

    QString s = arg + " =";
    ui->label->setText(s);

    connect(this,SIGNAL(initializeWith(const QString)),parent,SLOT(assignValue(const QString)));
}

DialogAggignParameter::~DialogAggignParameter()
{
    delete ui;
}

/**
 * @brief Ok
 */
void DialogAggignParameter::on_buttonBox_accepted()
{
    QString v = ui->labelValue->text();
    v.replace(" cm","");
    emit initializeWith(v);
}

/**
 * @brief Cancel
 */
void DialogAggignParameter::on_buttonBox_rejected()
{

}

/**
 * @brief 设置关键参数
 */
void DialogAggignParameter::on_radioButton1_clicked()
{
    QStringList sizes;
    sizes<<"H"<<"W"<<"CR"<<"L";

    bool ok;
    QString s = QInputDialog::getItem(this,"关键参数","选择尺寸",sizes,0,false,&ok);
    if(ok && !sizes.isEmpty())
        ui->labelValue->setText(s);
}

/**
 * @brief 设置常量参数
 */
void DialogAggignParameter::on_radioButton2_clicked()
{
    bool ok;
    double d = QInputDialog::getDouble(this,"常量参数","设置数值(单位:cm)",0.0,0,200,2,&ok);
    if(ok)
        ui->labelValue->setText(QString("%1 cm").arg(d));
}

/**
 * @brief 设置过程参数
 */
void DialogAggignParameter::on_radioButton3_clicked()
{

}
