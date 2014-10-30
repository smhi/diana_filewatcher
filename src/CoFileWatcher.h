/** @mainpage coFileWatcher
 * @author Martin Lilleeng Sætra <martinls@met.no>
 *
 * $Id: CoServer4.h,v 1.9 2007/09/04 10:34:45 martinls Exp $
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

#ifndef _COFILEWATCHER
#define _COFILEWATCHER

// Qt-includes
#include <QFileSystemWatcher>
#include <QObject>

#include <vector>
#include <map>

#ifdef HAVE_LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/basicconfigurator.h>
#else
#include <miLogger/logger.h>
#endif
#ifdef COSERVER
#include <coserver/miMessage.h>
#include <coserver/CoClient.h>
#else
#include <qUtilities/miMessage.h>
#include <qUtilities/CoClient.h>
#endif
#include "CoFileWatcherThread.h"
#include "CoConsole.h"

using namespace std;

#define POOL_SIZE 1

class CoFileWatcher: public QObject {
	Q_OBJECT

protected:
#ifdef HAVE_LOG4CXX
  log4cxx::LoggerPtr logger;
#endif

private:
  CoConsole *console;
  CoClient *coclient;
  //QFileSystemWatcher * fileWatcher;
  vector<CoFileWatcherThread*> threadPool;
  quint16 port;
  int id;
  bool visualMode;
  bool watchMode;
	bool indexMode;
  string index_path_;

public:
  /**
   * CoFileWatcher.
   * @param port Port to connect to
   * @param vm Run in visual (GUI) mode
   * @param dm Run in dynamic mode
   * @param logPropFile When given, log4cxx will use logPropFilename as properties file
   * @param logPropFilename The log4cxx properties file
   */
  CoFileWatcher(quint16 port, bool dm, string dir, bool vm, bool cp, string coserver_path, string index_path, bool im, bool logPropFile = false,
      string logPropFilename = "");
  ~CoFileWatcher();

public slots:
  void directoryHasChanged ( const QString & path );
  void fileHasChanged ( const QString & path, const bool & start_up );

};

#endif
