#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>
#include <libstkcomms.hpp>

#include "listener.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void beginProgramming();
    void update_progress_bar();

private:
    Ui::Dialog *ui;
    Listener *dongleListener_;
    QTimer *timer_;
    CStkComms* stk_;
};

#endif // DIALOG_H
