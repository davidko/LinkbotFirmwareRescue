#include "dialog.h"
#include "ui_dialog.h"
#include <libstkcomms.hpp>
#include <mobot.h>
#include <QDebug>
#include <QMessageBox>
#include <cassert>

std::string g_hexfilename;

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
#ifndef _WIN32
  g_hexfilename = std::string("hexfiles/linkbot_latest.hex");
#else
  /* Get the install path of BaroboLink from the registry */
  DWORD size;
  HKEY key;
  int rc;

  rc = RegOpenKeyExA(
      HKEY_LOCAL_MACHINE,
      "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\BaroboLink.exe",
      0,
      KEY_QUERY_VALUE,
      &key);

  if (ERROR_SUCCESS != rc) {
    return;
  }

  /* Find out how much memory to allocate. */
  rc = RegQueryValueExA(
      key,
      "PATH",
      NULL,
      NULL,
      NULL,
      &size);
  assert(ERROR_SUCCESS == rc);

  /* hlh: FIXME this should probably be TCHAR instead, and we should support
   * unicode or whatever */
  char* path = new char [size + 1];

  rc = RegQueryValueExA(
      key,
      "PATH",
      NULL,
      NULL,
      (LPBYTE)path,
      &size);
  assert(ERROR_SUCCESS == rc);

  path[size] = '\0';

  g_hexfilename = std::string(path) + "\\hexfiles\\linkbot_latest.hex";
  delete [] path;
  path = NULL;
#endif
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
  QMessageBox msgbox;
#ifdef _WIN32
    Sleep(3000);
#else
    usleep(3000000);
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
        usleep(100000);
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
  stk_->programAllAsync(g_hexfilename.c_str());
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
