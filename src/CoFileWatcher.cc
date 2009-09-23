/**
 * coFileWatcher
 * @author Martin Lilleeng S�tra <martinls@met.no>
 *
 * $Id: CoServer4.cc,v 1.22 2007/09/04 11:00:40 martinls Exp $
 *
 * Copyright (C) 2007 met.no
 *
 * Contact information:
 * Norwegian Meteorological Institute
 * Box 43 Blindern
 * 0313 OSLO
 * NORWAY
 * email: diana@met.no
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

// TODO: Add support for multiple servers active on different ports (on the same node)
// TODO: Add support for multiple clients per server

// Qt-includes
#include <QtNetwork>
#include <QTextStream>
#include <qapplication.h>

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include <qUtilities/QLetterCommands.h>
#include "CoFileWatcher.h"

CoFileWatcher::CoFileWatcher(quint16 port, bool dm, string dir, bool vm, bool cp, string coserver_path, bool logPropFile,
      string logPropFilename) :
  QObject()
{
  id = 0;
  visualMode = vm;
  watchMode = dm;
  //fileWatcher = NULL;
  threadPool.clear();
  cout << dir << endl;
  miString dirs(dir);
  vector<miString> dirlist = dirs.split(",");
  for (int i = 0; i < dirlist.size(); i++)
  {
	  CoFileWatcherThread * theThread = new CoFileWatcherThread();
	  if (theThread != NULL)
	  {
		  // Connect the threads with
		  // this object
		  connect( theThread, SIGNAL(directoryChanged(const QString&)), this, SLOT(directoryHasChanged(const QString&)));
		  connect( theThread, SIGNAL(fileChanged(const QString&)), this, SLOT(fileHasChanged(const QString&)));
		  threadPool.push_back(theThread);
	  }
  }

#ifdef HAVE_LOG4CXX
  /// LOGGER
  miString logpro;
  if (logPropFile) {
    logpro = logPropFilename;
  }
#endif

#ifdef HAVE_LOG4CXX
  if ( logpro.exists() ) {
    log4cxx::PropertyConfigurator::configure(logpro.c_str());
  } else {
    log4cxx::BasicConfigurator::configure();
    log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getWarn());
  }

  logger = log4cxx::Logger::getLogger("coserver4.CoServer4");
#endif
  cout << coserver_path.c_str() << endl;
  coclient = new CoClient(NULL, "filewatcher", "localhost", coserver_path.c_str());
  coclient->connectToServer();
  
  console = new CoConsole();
  if (visualMode) {
    console->show();
  }
  if (watchMode) {
	  //cout << "Watch: " << dir << endl;
#ifdef _DEBUG
	  cout << "Started watch mode: " << dir << endl;
#endif
	  // Unfortunately, this don't work with the current NFS implementation in linux.
	  /*
	  QString theDir((const char *)dir.c_str());
	  QStringList paths;
	  paths.append(theDir);
	  fileWatcher = new QFileSystemWatcher(paths);
	  QString thePath;
	  // Some magical Qt tricks...
	  connect( fileWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(directoryHasChanged(const QString&)));
	  connect( fileWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileHasChanged(const QString&)));
	  */

	  // add paths to the threads
	  // begin with one thread and one path
	  for (int i = 0; i < dirlist.size(); i++)
	  {
		  // start the listening
		  QString theDir((const char *)dirlist[i].c_str());
		  CoFileWatcherThread * theThread = threadPool[i];
		  theThread->addPath(theDir);
		  theThread->start();
	  }


  } // end watch mode

#ifdef HAVE_LOG4CXX
  if (isListening()) {
    LOG4CXX_INFO(logger, "coserver4 listening on port " << port);
  } else {
    LOG4CXX_ERROR(logger, "Failed to bind to port");
  }
#endif
  
  }

CoFileWatcher::~CoFileWatcher()
  {
	  for (int i = 0; i < threadPool.size(); i++)
	  {
		  // stop the listening
		  CoFileWatcherThread * theThread = threadPool[i];
		  theThread->stop = true;
		  theThread->wait();
		  delete theThread;
	  }
	  if (visualMode) {
		console->hide();
	  }
	  delete console;
  }

void CoFileWatcher::directoryHasChanged ( const QString & path )
{
    if (visualMode)
		console->log(path.toStdString());
	else
	{
		cout << string(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()+"\n").c_str();
		cout << string(path.toStdString() + "\n").c_str();
	}
	miMessage letter;
    letter.command= qmstrings::directory_changed;
    letter.commondesc= "directory changed";
    letter.common = path.toStdString();
    letter.to = qmstrings::all;
	coclient->sendMessage(letter);
} 
void CoFileWatcher::fileHasChanged ( const QString & path )
{
	if (visualMode)
		console->log(path.toStdString());
	else
	{
		cout << string(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()+"\n").c_str();
		cout << string(path.toStdString() + "\n").c_str();
	}
	miMessage letter;
    letter.command= qmstrings::file_changed;
    letter.commondesc= "file changed";
    letter.common = path.toStdString();
    letter.to = qmstrings::all;
	coclient->sendMessage(letter);
}

