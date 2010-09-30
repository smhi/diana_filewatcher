/** @file main.cc
 * Main class for coserver4. Only used for initialization.
 * Run with -gui for GUI.
 * @author Martin Lilleeng Sætra <martinls@met.no>
 */

// Qt-includes
#include <QApplication>
#include <stdlib.h>
#include <puTools/miCommandLine.h>
#include <qUtilities/QLetterCommands.h>
#include "CoFileWatcher.h"

#define _DEBUG

int main(int argc, char *argv[])
{
#ifdef _DEBUG
  cerr << "FileWatcher main program called" << endl;
#endif

#ifdef _DEBUG
  cerr << "argc: " << argc << endl;

  for(int i=0;i<argc;i++) {
    cerr << "argv: " << miutil::miString(argv[i]) << endl;
  }
#endif

  // parsing commandline-arguments
  vector<miCommandLine::option> opt(7);
  miutil::miString logfile;

  opt[0].flag = 'd';
  opt[0].alias = "dynamic";
  opt[0].hasArg = false;

  opt[1].flag = 'v';
  opt[1].alias = "visual";
  opt[1].hasArg = false;

  opt[2].flag = 'p';
  opt[2].alias = "port";
  opt[2].hasArg = true;

  opt[3].flag = 'L';
  opt[3].alias = "log4cxx-properties-file";
  opt[3].hasArg = true;

  opt[4].flag = 'w';
  opt[4].alias = "dir-to-watch";
  opt[4].hasArg = true;

  opt[5].flag = 'c';
  opt[5].alias = "coserver-path";
  opt[5].hasArg = true;

  miCommandLine cl(opt, argc, argv);

  quint16 port;
  quint16 fileport;

  if (cl.hasFlag('p')) {
    cerr << "P flag sent" << endl;
    //istringstream os((cl.arg('p'))[0]);
    //os >> port;

    if (cl.arg('p').size() >= 0) {
      port = miutil::miString(cl.arg('p')[0]).toInt(0);
    } else {
#ifdef _DEBUG
      cerr << "cl.arg('p').size() == 0" << cl.arg('p').size() << endl;
#endif
      port = qmstrings::port;
    }
/*  } else if (server->readPortFromFile(fileport) == 0) {
	  cerr << "Port read from file: " << fileport << endl;
	  port = fileport;
  //} else if (portFromRange == false) {
   */
  } else {
#ifdef _DEBUG
    cerr << "Flag p not set!!!"  << endl;
#endif
    port = qmstrings::port;
  }
  if (cl.arg('L').size() > 0) {
    logfile = cl.arg('L')[0];
  } else {
    logfile = "";
  }
  miutil::miString dir = "";
  if (cl.hasFlag('w')) {
    if (cl.arg('w').size() > 0)
      dir = cl.arg('w')[0];
  }

#ifdef _DEBUG
  cerr << "Port is really set to: " << port << endl;
#endif

  string logPropFilename = "";
  if (cl.hasFlag('L')) {
    if(cl.arg('L').size() > 0)
      logPropFilename = cl.arg('L')[0];
  }

  string coserver_path = "";

  if (cl.hasFlag('c')) {
    if(cl.arg('c').size() > 0)
      coserver_path = cl.arg('c')[0];
  }

  QCoreApplication* app;
  if (cl.hasFlag('v')) {
    cerr << "Visible" << endl;
    app = new QApplication(argc, argv);
  } else {
    cerr << "Console" << endl;
    app = new QCoreApplication(argc, argv);
  }

  CoFileWatcher watcher(port, cl.hasFlag('w'), dir, cl.hasFlag('v'), cl.hasFlag('c'), coserver_path, cl.hasFlag('L'), logPropFilename);
  /*
  if (!server->ready())
    exit(1);
  */
  return app->exec();
}
