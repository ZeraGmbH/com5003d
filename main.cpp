// test programm scpi parser

#include <iostream>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "com5003global.h"
#include "com5003d.h"


int main( int argc, char *argv[] )
{
    pid_t pid;
   openlog(ServerBasisName, LOG_PID, LOG_DAEMON); // verbindung zum syslogd aufnehmen
    
   cCom5003Server* com5003d=new cCom5003Server(); // das ist der server
   
   // überprüfen ob default einstellungen überschrieben werden sollen
   int r=0;  
   if ( ( argc > 1 ) && ( com5003d->sDebugLevel(argv[1]) ) ) { //  debug level 0..7
       syslog(LOG_EMERG,"illegal debug level\n") ; 
       r=1;
   } 	

   if ( ( argc > 2 ) && ( com5003d->SetServerNr(argv[2]) ) ) { //  neue com5003 karten nummer
       syslog(LOG_EMERG,"illegal pcb nr\n") ; 
       r=1;
   } 	

   if ( ( argc > 3 ) && ( com5003d->SetI2CSlaveAdr(argv[3]) ) ) { //  neue i2c slave adresse
       syslog(LOG_EMERG,"illegal i2c slave adress\n") ; 
       r=1;
   } 

   if ( ( argc > 4 ) && ( com5003d->SetI2CMasterAdr(argv[4]) ) ) { //  neue i2c master adresse
       syslog(LOG_EMERG,"illegal i2c master adress\n") ;
       r=1;
   } 

   if ( ( argc > 5 ) && ( com5003d->SetI2CDevNode(argv[5]) ) ) { // neuer device knoten
       syslog(LOG_EMERG,"illegal i2c device node\n");
       r=1;
   }
    
   if (r==0) { // nur wenn bis jetzt kein fehler aufgetreten ist
#ifndef WMDEBUG       
       if ( (pid=fork() ) < 0 ) { // kindprozess generieren 
	   syslog(LOG_EMERG,"fork() failed\n") ; // fehlermeldung falls nicht
	   return (1);
       }
	
       if (pid==0) { // wenn es der kindprozess ist
       syslog(LOG_INFO,"com5003d server child process created\n");
	   chdir ("/"); // bekommt er einen platz zum "leben"
	   setsid();
	   close (STDIN_FILENO); // löst sich von der konsole
	   close (STDOUT_FILENO);
	   close (STDERR_FILENO);
	   open ("/dev/null",O_RDWR);
	   dup (STDIN_FILENO);
	   dup (STDOUT_FILENO);
       r=com5003d->Execute(); // und läuft von nun an eigenständig
       syslog(LOG_INFO,"com5003d server child process terminated\n");
       }
#else
       r=com5003d->Execute(); // wenn DEBUG -> kein fork -> server läuft im vordergrund
#endif // WMDEBUG	       
   }
   closelog();
   return (r);
}
