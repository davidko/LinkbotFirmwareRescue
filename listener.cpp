#include "listener.h"
#include <mobot.h>

Listener::Listener(QObject *parent) : QObject(parent)
{
  timer_ = new QTimer(this);
  QObject::connect(timer_, SIGNAL(timeout()), 
      this, SLOT(doWork()));
}

void Listener::startWork()
{
  timer_->start();
}

void Listener::doWork()
{
  static int lastStatus = 0;
  char buf[1024];
  int rc;
  rc = Mobot_dongleGetTTY(buf, 1024);
  if(
      (0 == rc) &&
      (0 != lastStatus)
    ) 
  {
    emit dongleDetected(QString(buf));
    timer_->stop();
  }
  lastStatus = rc;
}
