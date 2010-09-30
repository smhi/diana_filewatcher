/*
Diana - A Free Meteorological Visualisation Tool

$Id: diRoadRoadObsManager.cc,v 2.2 2006/05/29 15:00:31 lisbethb Exp $

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
//#define METNOOBS
//#define BUFROBS

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <QDir>
#include <QDateTime>

#include "CoFileWatcherThread.h"

using namespace std;


// Default constructor
CoFileWatcherThread::CoFileWatcherThread()
:QThread(NULL){
	stop = false;
	_directory = "";
	_file = "";
	_watch_file = "";
}

void CoFileWatcherThread::addPath ( const QString & path )
{
	QMutexLocker l(&jobMutex);
	// Check for pattern matching...
	if (path.contains("*")||path.contains("?")||path.contains("[")||path.contains("]"))
		_file = path;
	else
		_directory = path;
}

QString CoFileWatcherThread::directory ()
{
	QMutexLocker l(&jobMutex);
	return _directory;
}

QString CoFileWatcherThread::file ()
{
	QMutexLocker l(&jobMutex);
	return _file;
}


CoFileWatcherThread::~CoFileWatcherThread(){
}


void CoFileWatcherThread::run()
{
	// the main loop...
	// WE must poll...
	// and make a thread of it
	struct stat buf;
	// init the dir time
	time_t st_prev_mtime = time(NULL);
	// init the file time
	time_t st_prev_file_mtime = time(NULL);

	// init the watch_files time
	time_t st_prev_wfile_mtime = time(NULL);

	while (!stop)
	{
		// we must ensure consistency of the object
		QMutexLocker l(&jobMutex);

		if (_directory != "")
		{
			if(!stat(_directory.toStdString().c_str(), &buf))
			{
				// > to deal with start up

				if (buf.st_mtime > st_prev_mtime)
				{
					QString theDir(_directory);
					emit directoryChanged(theDir);
					st_prev_mtime = buf.st_mtime;

				}
			}
		}
		if (_file != "")
		{
			// split _files in dir and pattern part
			QFileInfo fi(_file);
			QString basename = fi.completeBaseName();
			QString suffix = fi.completeSuffix();
			QString pattern = basename + "." + suffix;
			QString cwd = fi.absolutePath();
			//cout << cwd.toStdString().c_str() << endl;
			if(!stat(cwd.toStdString().c_str(), &buf))
			{
				// > to deal with start up
				// the directory representing file...
				if (buf.st_mtime > st_prev_file_mtime)
				{
					QString theDir(cwd);
					// send both dir changed and file changed...
					// send dir changed
					emit directoryChanged(theDir);
					st_prev_file_mtime = buf.st_mtime;
					sleep(1);
					// list the directory
					// search for the youngest file
					// here, we must check for creation time
					QDir d;
					d.setSorting(QDir::Time);
					d.cd(cwd);
					QStringList patterns(pattern);
					QFileInfoList list = d.entryInfoList(patterns);
					if (list.size() > 0)
					{
						// only the youngest file is of interest
						// HERE, we must check for creation time
						// NOTE: there can be other files with other extensions
						// in the file system.
						for (int k = 0; k < list.size(); k++)
						{
							QFileInfo fileInfo = list.at(k);
							QString theFile = fileInfo.absoluteFilePath();
							QDateTime qmtime(fileInfo.lastModified());
							time_t mtime = qmtime.toTime_t();
							// Note, the 'old' file can fullfill this
							// so we must not break here
							// and we must check that its a new file...
							if (theFile != _watch_file)
							{
								if ((mtime >= st_prev_file_mtime) || (mtime >= st_prev_wfile_mtime))
								{
									_watch_file = theFile;
									st_prev_wfile_mtime = mtime;
									// send file changed...
									emit fileChanged(theFile);
								}
							}
						}
					}
				}
				else
				{
					// stat the individual file
					// if missing, list the directory...
					// thats the case when starting up....
					if (_watch_file == "")
					{
						// split _files in dir and pattern part
						QFileInfo fi(_file);
						QString basename = fi.completeBaseName();
						QString suffix = fi.completeSuffix();
						QString pattern = basename + "." + suffix;
						QString cwd = fi.absolutePath();
						QDir d;
						d.setSorting(QDir::Time);
						d.cd(cwd);
						QStringList patterns(pattern);
						QFileInfoList list = d.entryInfoList(patterns);
						if (list.size()> 0)
						{
							// only the youngest file is of interest
							// HERE, we must check for creation time
							// NOTE: there can be other files with other extensions
							// in the file system.
							for (int k = 0; k < list.size(); k++)
							{
								QFileInfo fileInfo = list.at(k);
								QString theFile = fileInfo.absoluteFilePath();
								QDateTime qmtime(fileInfo.lastModified());
								time_t mtime = qmtime.toTime_t();
								// Note, the 'old' file can fullfill this
								// so we must not break here
								// and we must check that its a new file...
								if (theFile != _watch_file)
								{
									if ((mtime >= buf.st_mtime) || (mtime >= st_prev_wfile_mtime))
									{
										_watch_file = theFile;
										st_prev_wfile_mtime = mtime;
										// send file changed...
										emit fileChanged(theFile);
									}
								}
							}

							// only the youngest file is of interest
							// NOTE: there can be other files with other extensions
							// in the file system.
							/*
							QFileInfo fileInfo = list.at(0);
							QString theFile = fileInfo.absoluteFilePath();
							QDateTime qmtime(fileInfo.lastModified());
							time_t mtime = qmtime.toTime_t();
							_watch_file = theFile;
							st_prev_wfile_mtime = mtime;
							// send file changed...
							emit fileChanged(theFile);
							*/
						}
					}
					else
					{
						if(!stat(_watch_file.toStdString().c_str(), &buf))
						{
							// > to deal with start up
							if (buf.st_mtime > st_prev_wfile_mtime)
							{
								QString theFile(_watch_file);
								emit fileChanged(theFile);
								st_prev_wfile_mtime = buf.st_mtime;

							}
						}
					}
				}
			}
		}
		// The resolution of stat i on per second basis, there is no need to poll more frequently
		sleep(1);
	}
	exit();
}
