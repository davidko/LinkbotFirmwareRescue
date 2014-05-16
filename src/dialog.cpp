#include "dialog.h"
#include "ui_dialog.h"
#include <libstkcomms.hpp>
#include <mobot.h>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <cassert>

Dialog::Dialog(QWidget *parent, QString hexfilename) :
    QDialog(parent),
    ui(new Ui::Dialog),
    hexfilename_(hexfilename)
{
  ui->setupUi(this);

  /* Find and replace the firmware version text template with the actual
   * firmware version. Kinda hacky. */
  auto html = ui->textBrowser->toHtml();
  html.replace("{{FIRMWARE_VERSION}}", QFileInfo(hexfilename).completeBaseName());
  ui->textBrowser->setHtml(html);

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
  int numTries = 0;
  char buf[1024];
  int rc;
  qDebug() << "Begin Programming.";
  QMessageBox msgbox;
#ifdef _WIN32
    Sleep(2000);
#else
#ifndef __APPLE__
    usleep(2000000);
#endif
#endif
  rc = Mobot_dongleGetTTY(buf, 1024);
  if(rc) {
    msgbox.setText("No connected robots detected.");
    msgbox.exec();
    return;
  }
  while(1) {
    qDebug() << "Connecting to: " << buf;
    stk_ = new CStkComms();
    rc = stk_->connectWithTTY(buf);
    qDebug() << "Connection finish with code: " << rc;
    if(rc) { 
      if(numTries > 20) {
        msgbox.setText("Detected robot not running valid bootloader. Please make "
            "sure that the \"Program Firmware\" button is being clicked after "
            "the red LED flashes but before the solid blue LED is showing. 1");
        msgbox.exec();
        return; 
      } else {
#ifdef _WIN32
        Sleep(100);
#else
#ifndef __MACH__
        usleep(100000);
#endif
#endif
        numTries++;
      }
    } else {
      break;
    }
    numTries = 0;
#if 0
    qDebug() << "Attempting handshake...";
    rc = stk_->handshake();
    qDebug() << "Handshake finished with code " << rc;
    if(rc) { 
      if(numTries > 5) {
        msgbox.setText("Detected robot not running valid bootloader. Please make "
            "sure that the \"Program Firmware\" button is being clicked after "
            "the red LED flashes but before the solid blue LED is showing. 2");
        msgbox.exec();
        return; 
      } else {
#ifdef _WIN32
        Sleep(500);
#else
        usleep(500000);
#endif
        numTries++;
      }
    }
#endif
  }
  qDebug() << "Attempting to begin programming...";
  stk_->programAllAsync(hexfilename_.toStdString().c_str());
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
    stk_->disconnect();
    dongleListener_->startWork();
  } else {
    ui->progressBar->setValue(progress*100);
  }
}
