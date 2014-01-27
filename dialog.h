#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>
#include <libstkcomms.hpp>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_pushButton_clicked();
    void update_progress_bar();

private:
    Ui::Dialog *ui;
    QTimer *timer_;
    CStkComms* stk_;
};

#endif // DIALOG_H
