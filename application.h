#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtCore>
#include <QCoreApplication>

class cApplication: public QCoreApplication
{
    Q_OBJECT
public:
    cApplication(int &argc, char** argv)
        :QCoreApplication(argc, argv)
    {
        QTimer::singleShot(0, this, SIGNAL(appStarting()));
    }

signals:
    void appStarting();
};

#endif // APPLICATION_H
