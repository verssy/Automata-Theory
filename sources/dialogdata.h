#ifndef DIALOGDATA_H
#define DIALOGDATA_H

#include <QDialog>

#include "Config.h"

namespace Ui {
class DialogData;
}

class DialogData : public QDialog
{
    Q_OBJECT

    Config& config = Config::instance();

public:
    explicit DialogData(QWidget *parent = nullptr);
    ~DialogData();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogData *ui;
};

#endif // DIALOGDATA_H
