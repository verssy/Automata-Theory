#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->addAction("Автор");
    ui->menuBar->addAction("Тема");
    ui->menuBar->addAction("Данные");
    ui->menuBar->addAction("Запись в файл");
    ui->menuBar->addAction("Выход");
    connect(ui->menuBar->actions()[0], &QAction::triggered, this, &MainWindow::action_author_clicked);
    connect(ui->menuBar->actions()[1], &QAction::triggered, this, &MainWindow::action_task_clicked);
    connect(ui->menuBar->actions()[2], &QAction::triggered, this, &MainWindow::action_data_clicked);
    connect(ui->menuBar->actions()[3], &QAction::triggered, this, &MainWindow::action_write_clicked);
    connect(ui->menuBar->actions().back(), &QAction::triggered, this, &MainWindow::action_exit_clicked);
    if (config.load_from_file) {
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->lineEdit_3->setEnabled(false);
    }
    else {
        ui->lineEdit->setEnabled(true);
        ui->lineEdit_2->setEnabled(true);
        ui->lineEdit_3->setEnabled(true);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::action_author_clicked()
{
    QMessageBox qmb(this);
    qmb.setWindowTitle("Автор");
    qmb.setText("Дубровин Андрей ИП-917");
    qmb.exec();
}

void MainWindow::action_task_clicked()
{
    QMessageBox qmb(this);
    qmb.setWindowTitle("Тема");
    qmb.setText("Написать программу, которая по предложенному описанию языка построит детерминированный конечный автомат, распознающий этот язык, и проверит вводимые с клавиатуры цепочки на их принадлежность языку. Предусмотреть возможность поэтапного отображения на экране процесса проверки цепочек. Функция переходов ДКА может изображаться в виде таблицы и графа (выбор вида отображения посредством меню).\n\nВариант 3:\n\nАлфавит, обязательная фиксированная подцепочка и кратность длины всех цепочек языка.");
    qmb.exec();
}

void MainWindow::action_data_clicked()
{
    DialogData dd(this);
    dd.setModal(true);
    dd.exec();
    if (config.load_from_file) {
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->lineEdit_3->setEnabled(false);
    }
    else {
        ui->lineEdit->setEnabled(true);
        ui->lineEdit_2->setEnabled(true);
        ui->lineEdit_3->setEnabled(true);
    }
}

void MainWindow::action_write_clicked()
{
    ofstream out(config.output_file_path);
    if (out.is_open()) {
        if (dka_creator == nullptr) {
            output_error("ДКА не построен");
            return;
        }
        auto params = dka_creator->get_input_params();
        out << params.alphabet << " " << params.substr << " " << params.k << " " << params.filename << std::endl;
        output_error("Записано");
    }
    else {
        output_error("Файл не открылся. беда");
    }
}

void MainWindow::action_exit_clicked()
{
    exit(0);
}

void MainWindow::on_pushButton_clicked()
{
    while (ui->tableWidget->columnCount())
        ui->tableWidget->removeColumn(0);
    while (ui->tableWidget->rowCount())
        ui->tableWidget->removeRow(0);
    if (dka_creator != nullptr) {
        delete dka_creator;
        dka_creator = nullptr;
    }
    std::thread th(&MainWindow::init_dka_creator, this);
    th.detach();
}

void MainWindow::on_pushButton_2_clicked()
{
    std::thread th(&MainWindow::create_dka_presentation, this);
    th.detach();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->plainTextEdit->clear();
    if (dka_creator == nullptr) {
        output_error("ДКА не построен");
        return;
    }
    if (dka_creator->check(ui->lineEdit_4->text().toStdString())) {
        output_error("Ага, подходит!");
        ui->plainTextEdit->setPlainText(QString::fromStdString(dka_creator->get_hist(ui->lineEdit_4->text().toStdString())));
    }
    else {
        output_error("Не подходит :(");
    }
}

void MainWindow::init_dka_creator()
{
    output_error("");
    try {
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        if (config.load_from_file) {
            ifstream input(config.input_file_path);
            if (input.is_open()) {
                try {
                    DkaCreator::InputParams params;
                    input >> params.alphabet >> params.substr >> params.k >> params.filename;
                    dka_creator = new DkaCreator(params.alphabet, params.substr, params.k, params.filename);
                    output_error("ДКА загружен из файла");
                } catch (exception& ex) {
                    output_error(ex.what());
                }
            }
            else {
                output_error(QString::fromStdString("Нет файла " + config.input_file_path));
            }
        }
        else {
            if (ui->lineEdit_3->text().length() == 0)
                output_error("Введите k");
            else if (ui->lineEdit->text().length() == 0)
                output_error("Алафвит пустой");
            else {
                dka_creator = new DkaCreator(ui->lineEdit->text().toStdString(), ui->lineEdit_2->text().toStdString(), ui->lineEdit_3->text().toInt(), config.dka_file_path);
                output_error("ДКА сгенерирован");
            }
        }
    } catch (exception& ex) {
        output_error(ex.what());
    }
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    if (dka_creator != nullptr) {
        std::map<char, int> indexes;
        DkaCreator::TablePresentation table_presentation = dka_creator->get_table_presentation();
        for (auto &i : table_presentation.alphabet) {
            ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
            ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem(QString::fromStdString(std::string(1, i))));
            indexes[i] = ui->tableWidget->columnCount() - 1;
        }
        for (auto &i : table_presentation.states) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            ui->tableWidget->setVerticalHeaderItem(ui->tableWidget->rowCount() - 1, new QTableWidgetItem(QString::fromStdString(i)));
        }
        for (auto &i : table_presentation.transitions)
            for (auto &j : i.second)
                ui->tableWidget->setItem(i.first, indexes[j.first], new QTableWidgetItem(QString::fromStdString(j.second)));
    }
}

void MainWindow::create_dka_presentation()
{
    if (dka_creator == nullptr) {
        output_error("ДКА не построен");
    }
    else {
        auto dka_copy = *dka_creator;
        dka_copy.gen_image();
    }
}

void MainWindow::handle_config()
{
    if (config.load_from_file) {
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->lineEdit_3->setEnabled(false);
    }
    else {
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->lineEdit_3->setEnabled(false);
    }
}

void MainWindow::output_error(QString message)
{
    ui->label_6->setText(message);
}
