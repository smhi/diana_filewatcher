bin_PROGRAMS = filewatcher

qt_sources = \
	CoConsole.cc \
	CoFileWatcher.cc \
	CoFileWatcherThread.cc



qt_headers = $(qt_sources:.cc=.h)

nodist_filewatcher_SOURCES = $(qt_sources:.cc=_moc.cc)

filewatcher_SOURCES =   \
	main.cc       \
	$(qt_headers) \
	$(qt_sources)

filewatcher_CPPFLAGS = -DNOLOG4CXX
filewatcher_CXXFLAGS = \
	$(QT4_CPPFLAGS) \
	$(QUTILITIES_CPPFLAGS) \
	$(PUTOOLS_CPPFLAGS) \
	$(MILOGGER_CPPFLAGS)
filewatcher_LDFLAGS = \
	$(QT4_LDFLAGS) \
	$(QUTILITIES_LDFLAGS) \
	$(PUTOOLS_LDFLAGS) \
	$(MILOGGER_LDFLAGS)
filewatcher_LDADD = \
	$(QT4_LIBS) \
	$(QUTILITIES_LIBS) \
	$(PUTOOLS_LIBS) \
	$(MILOGGER_LIBS)

filewatcher_CPPFLAGS += \
  ${COSERVER_CPPFLAGS}
filewatcher_LDFLAGS += \
	${COSERVER_LDFLAGS}
filewatcher_LDADD += \
	${COSERVER_LIBS}


# This lets GNU make create any moc_*.cpp from the equivalent *.h
%_moc.cc: %.h
	$(MOC4) $< -o $@
