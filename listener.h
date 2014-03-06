#ifndef LISTENER_H__
#define LISTENER_H__

#include <QObject>
#include <QTimer>

class Listener : public QObject
{
  Q_OBJECT
  public:
    Listener(QObject *parent = 0);

  public slots:
    void startWork();
    void doWork();

  signals:
    void dongleDetected(const QString &port);

  private:
    QTimer *timer_;
};

#endif
