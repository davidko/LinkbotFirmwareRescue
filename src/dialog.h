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
    Dialog(QWidget *parent, QString hexfilename);
    ~Dialog();

public slots:
    void beginProgramming();
    void update_progress_bar();

private:
    Ui::Dialog *ui;
    Listener *dongleListener_;
    QTimer *timer_;
    CStkComms* stk_;

    QString hexfilename_;
};

#endif // DIALOG_H
