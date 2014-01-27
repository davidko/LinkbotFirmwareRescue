#include "dialog.h"
#include "ui_dialog.h"
#include <libstkcomms.hpp>
#include <mobot.h>
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
  char buf[1024];
  int rc;
  rc = Mobot_dongleGetTTY(buf, 1024);
  if(rc) {
    return;
  }
  stk_ = new CStkComms();
  rc = stk_->connectWithTTY(buf);
  if(rc) { return; }
  stk_->programAllAsync("hexfile.hex");
  ui->progressBar->setEnabled(true);
  /* Start a timer to update the progress bar */
  timer_ = new QTimer(this);
  QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(update_progress_bar()));
  timer_->start(1000);
  ui->pushButton->setEnabled(false);
}

void Dialog::update_progress_bar()
{
  //qDebug() << "Progress at: " << stk_->getProgress();
  double progress = stk_->getProgress();
  qDebug() << progress;
  if(progress >= 1.0) {
    ui->pushButton->setEnabled(true);
    ui->progressBar->reset();
    ui->progressBar->setEnabled(false);
    timer_->stop();
    qDebug() << "Update finished.";
  } else {
    ui->progressBar->setValue(progress*100);
  }
}
