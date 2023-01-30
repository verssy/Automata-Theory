#include "dialogdata.h"
#include "ui_dialogdata.h"

DialogData::DialogData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogData)
{
    ui->setupUi(this);
    if (config.load_from_file)
        ui->radioButton->setChecked(true);
    else
        ui->radioButton_2->setChecked(true);
    ui->lineEdit->setText(QString::fromStdString(config.input_file_path));
    ui->lineEdit_2->setText(QString::fromStdString(config.dka_file_path));
    ui->lineEdit_3->setText(QString::fromStdString(config.output_file_path));
}

DialogData::~DialogData()
{
    delete ui;
}

void DialogData::on_pushButton_clicked()
{
    if (ui->radioButton->isChecked())
        config.load_from_file = true;
    else
        config.load_from_file = false;
    if (ui->lineEdit->text().length() > 0)
        config.input_file_path = ui->lineEdit->text().toStdString();
    if (ui->lineEdit_2->text().length() > 0)
        config.dka_file_path = ui->lineEdit_2->text().toStdString();
    if (ui->lineEdit_3->text().length() > 0)
        config.output_file_path = ui->lineEdit_3->text().toStdString();
    this->close();
}
