#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>

#include <thread>
#include <string>
#include <map>

#include "DkaCreator.h"
#include "dialogdata.h"
#include "Config.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Config& config = Config::instance();
    DkaCreator* dka_creator = nullptr;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void action_author_clicked();
    void action_task_clicked();
    void action_data_clicked();
    void action_write_clicked();
    void action_exit_clicked();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
    void create_dka_presentation();
    void handle_config();
    void output_error(QString);
    void init_dka_creator();

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
