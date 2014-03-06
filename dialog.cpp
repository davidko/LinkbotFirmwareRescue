#include "dialog.h"
#include "ui_dialog.h"
#include <libstkcomms.hpp>
#include <mobot.h>
#include <QDebug>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    dongleListener_ = new Listener(this);
    QObject::connect(dongleListener_, SIGNAL(dongleDetected(const QString&)),
        this, SLOT(beginProgramming()));
    dongleListener_->startWork();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::beginProgramming()
{
  char buf[1024];
  int rc;
  rc = Mobot_dongleGetTTY(buf, 1024);
  QMessageBox msgbox;
  if(rc) {
    msgbox.setText("No connected robots detected.");
    msgbox.exec();
    return;
  }
  stk_ = new CStkComms();
  rc = stk_->connectWithTTY(buf);
  if(rc) { 
    msgbox.setText("Detected robot not running valid bootloader. Please make "
        "sure that the \"Program Firmware\" button is being clicked after "
        "the red LED flashes but before the solid blue LED is showing.");
    msgbox.exec();
    return; 
  }
  rc = stk_->handshake();
  if(rc) { 
    msgbox.setText("Detected robot not running valid bootloader. Please make "
        "sure that the \"Program Firmware\" button is being clicked after "
        "the red LED flashes but before the solid blue LED is showing.");
    msgbox.exec();
    return; 
  }
  stk_->programAllAsync("hexfile.hex");
  ui->progressBar->setEnabled(true);
  /* Start a timer to update the progress bar */
  timer_ = new QTimer(this);
  QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(update_progress_bar()));
  timer_->start(1000);
}

void Dialog::update_progress_bar()
{
  //qDebug() << "Progress at: " << stk_->getProgress();
  double progress = stk_->getProgress();
  //qDebug() << progress;
  if(progress >= 1.0) {
    ui->progressBar->reset();
    ui->progressBar->setEnabled(false);
    timer_->stop();
    dongleListener_->startWork();
  } else {
    ui->progressBar->setValue(progress*100);
  }
}
