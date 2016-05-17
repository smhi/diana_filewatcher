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
#include <map>
#include <QDir>
#include <QDateTime>
#include <puTools/TimeFilter.h>


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

void CoFileWatcherThread::setDaysBack( int days_back )
{
	QMutexLocker l(&jobMutex);
	_days_back = days_back;
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
	time_t st_prev_mtime = 0;
	// init the file (directory representing the file) time
	time_t st_prev_file_mtime = 0;
	// init the watch_files time
	time_t st_prev_wfile_mtime = 0;
    // time last accesed
    time_t time_last_accessed = 0;
	// The QDateTime specifier
	QString datetimespec = "";
	
	std::map<QString,QFileInfo> prevDirList;
	
	int seconds_back = _days_back*24*3600;

  bool start_up = true;
	while (!stop)
	{
		// we must ensure consistency of the object
		QMutexLocker l(&jobMutex);
		
		time_t time_now = time(NULL);

		if (_directory != "")
		{
			if(!stat(_directory.toStdString().c_str(), &buf))
			{
				// when start up, this will always bee true
				if (buf.st_mtime > st_prev_mtime)
				{
					st_prev_mtime = buf.st_mtime;
					QString theDir(_directory);
					emit directoryChanged(theDir);
				}
			}
		}
		if (_file != "")
		{
      // split _files in dir and pattern part
      if (_file.contains("[") && _file.contains("]"))
      {
        // init time filter and replace yyyy etc. with ????
        miutil::TimeFilter tf;
        std::string sourcestr = _file.toStdString();
        std::string part1, part2=sourcestr;
        if(sourcestr.find("/") != sourcestr.npos) {
          part1 = sourcestr.substr(0,sourcestr.find_last_of("/")+1);
          part2 = sourcestr.substr(sourcestr.find_last_of("/")+1,sourcestr.size()-1);
        }
        tf.initFilter(part2);
        sourcestr = part1 + part2;
        _file = sourcestr.c_str();
      }
			
			//	// filespec contains a QDataTime specifier...
			//	// get it, and replace with '*'.
			//	QString left("[");
			//	QString right("]");
			//	int start = _file.indexOf(left);
			//	int stop = _file.indexOf(right);
			//	int len = stop - start;
			//	//cerr << _file.toStdString().c_str() << endl;
			//	datetimespec = _file.mid(start + 1, len - 1);
			//	//cerr << datetimespec.toStdString().c_str() << endl;
			//	QString str_to_replace = "[" + datetimespec + "]";
			//	_file.replace(str_to_replace, QString("*"));
			//	//cerr << _file.toStdString().c_str() << endl;
			//}
			QFileInfo fi(_file);
			QString basename = fi.completeBaseName();
			QString suffix = fi.completeSuffix();
      // we must check i suffix is empty...
      QString pattern;
      if (suffix.length() == 0) {
        pattern = basename;
      }
      else{
        pattern = basename + "." + suffix;
      }
	  //cout << "pattern: " << pattern.toStdString() << endl;
			QString cwd = fi.absolutePath();
			//cout << cwd.toStdString().c_str() << endl;
			if(!stat(cwd.toStdString().c_str(), &buf))
			{
				// when start up, this will always bee true
				// the directory representing file...
				if (buf.st_mtime > st_prev_file_mtime)
				{
					// send both dir changed and file changed...
					// send dir changed
					st_prev_file_mtime = buf.st_mtime;
					QString theDir(cwd);
					emit directoryChanged(theDir);
					// list the directory
					// search for the youngest file
					// here, we must check for creation time
					QDir d;
					d.setSorting(QDir::Time);
					d.cd(cwd);
					QStringList patterns(pattern);
					QFileInfoList list = d.entryInfoList(patterns);
					std::map<QString,QFileInfo> dirList;
					// directory not empty
					if (list.size() > 0)
					{
						// only the youngest file is of interest
						// HERE, we must check for either creation time or time stamp, if present...
						// NOTE: there can be other files with other extensions
						// in the file system.
						time_t prev_mtime = 0;
						time_t mtime = 0;
						int index = 0;
						QDateTime qtimestamp;
						QDateTime qmtime;
						
						for (int k = 0; k < list.size(); k++)
						{
							QFileInfo fileInfo = list.at(k);
							QString theFile = fileInfo.absoluteFilePath();
							// Insert in to map...
							dirList[theFile] = fileInfo;
						}
						// Iterate through the map and compare with prev map...
						// File not present in the prev map is new..
						std::map<QString,QFileInfo>::iterator diter = dirList.begin();
						for (; diter != dirList.end(); diter++) {
							// New file found
							if (prevDirList.count(diter->first) == 0) {
								// More general, but perhaps not working
								qmtime = diter->second.created();
								mtime = qmtime.toTime_t();
								// Check for youngest file
								if (mtime > prev_mtime)
								{
									prev_mtime = mtime;
									st_prev_wfile_mtime = prev_mtime;
									_watch_file = diter->first;
								}
								if (time_now - time_last_accessed > 3600) {
									start_up = true;
									time_last_accessed = time_now;
								}

								// send file changed...
								// limit with seconds_back.
								if (seconds_back > 0)
								{
									if (time_now - mtime <= seconds_back)
									{
										emit fileChanged(diter->first, start_up);
									}
										
								}
								else
								{
									emit fileChanged(diter->first, start_up);
								}
								start_up = false;
								
							}
						}
						prevDirList = dirList;

					}
				}
				else
				{

					if(!stat(_watch_file.toStdString().c_str(), &buf))
					{
						// > to deal with start up
						if (buf.st_mtime > st_prev_wfile_mtime)
						{
							if (time_now - time_last_accessed > 3600) {
								start_up = true;
								time_last_accessed = time_now;
							}
							st_prev_wfile_mtime = buf.st_mtime;
							QString theFile(_watch_file);
							emit fileChanged(theFile, start_up);
							start_up = false;
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
