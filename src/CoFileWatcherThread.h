/*
  Diana - A Free Meteorological Visualisation Tool

  $Id: diRoadObsManager.h,v 2.0 2006/05/24 14:06:23 audunc Exp $

  Copyright (C) 2006 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no
  
  This file is part of Diana

  Diana is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Diana is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Diana; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef cofilewatcherthread_h
#define cofilewatcherthread_h
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QString>

/**

  \brief Managing directories and files
  


*/

class CoFileWatcherThread: public QThread {

Q_OBJECT

private:
  // must be on a per thread basis
  // In order to avoid unneccesary complexity,
  // each thread only deals with one directory or file.
  QMutex jobMutex;
  QString _directory;
  QString _file;
  QString _watch_file;

public:
  bool stop;
  CoFileWatcherThread();
  ~CoFileWatcherThread();
  void run();
  void addPath ( const QString & path );
  QString directory ();
  QString file ();
signals:

  // Qt take care of generating the implementation
  void directoryChanged ( const QString & path );
  void fileChanged ( const QString & path, const bool & start_up );


};

#endif
