// test programm scpi parser

#include <iostream>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "application.h"
#include "com5003dglobal.h"
#include "com5003d.h"

cApplication* app;

int main( int argc, char *argv[] )
{
    pid_t pid;
    openlog(ServerName, LOG_PID, LOG_DAEMON); // open connection to syslogd

    app = new cApplication (argc, argv);
    cCOM5003dServer* com5003d=new cCOM5003dServer(); // this is our server

    int r;

#ifndef COM5003DDEBUG
    if ( (pid=fork() ) < 0 ) // we generate a child process
    {
        syslog(LOG_EMERG,"fork() failed\n") ; // error message to syslogd if not
        return (1);
    }

    if (pid==0) // if we have a child process now
    {
        syslog(LOG_INFO,"com5003d server child process created\n");
        chdir ("/"); // it becomes a place to live
        setsid();
        close (STDIN_FILENO); // we disconnect from std io and std error o
        close (STDOUT_FILENO);
        close (STDERR_FILENO);
        open ("/dev/null",O_RDWR); // a null device for ev. created output
        dup (STDIN_FILENO);
        dup (STDOUT_FILENO);
        r = app->exec(); // and runs as daemon now

    }
#else
        r = app->exec(); // id DEBUG -> no fork -> server runs in foreground
#endif // COM5003DDEBUG

    if (r == parameterError)
        syslog(LOG_EMERG,"Abort, wrong parameter count\n") ;
    if (r == xsdfileError)
        syslog(LOG_EMERG,"Abort, xsd file error\n") ;
    if (r == xmlfileError)
        syslog(LOG_EMERG,"Abort, xml file error\n") ;
    if (r == atmelError)
        syslog(LOG_EMERG,"Abort, atmel not running\n") ;

    syslog(LOG_INFO,"com5003d server child process terminated ret = %d\n", r);
    delete com5003d;
    closelog();
    return (r);
}
