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
        QTimer::singleShot(0, this, SLOT(StartApp()));
    }

signals:
    void appStarting();

private slots:
    void StartApp(){emit appStarting();}
};

#endif // APPLICATION_H
