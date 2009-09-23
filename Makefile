# includefile contains Compiler definitions
include ../conf/${OSTYPE}.mk

DEFINES=$(LOGGDEF)

SRCDIR=src
LIBDIR=lib$(PLT)
OBJDIR=obj$(PLT)
BINDIR=bin$(PLT)
LOCALINC=$(LOCALDIR)/include/
INCDIR=../include

INCLUDE= -I$(INCDIR) \
	 -I$(LOCALINC)	\
     $(QTINCLUDE)

LINKS= -L$(LOCALDIR)/$(LIBDIR) -lqUtilities -lpuTools $(QTLIBDIR) $(QT_LIBS) $(LOGGLIBS) $(XLIBDIR) -lXext -lXt -lX11 -lm

DEPENDSFILE=$(OBJDIR)/make.depends
MOCFILE=$(OBJDIR)/make.moc


OPTIONS="LEX=${LEX}" "CXX=${CXX}" "CCFLAGS=${CXXFLAGS} ${DEFINES}" "CC=${CC}" "CFLAGS=${CFLAGS} ${DEFINES}" "LDFLAGS=${CXXLDFLAGS}" "AR=${AR}" "ARFLAGS=${ARFLAGS}" "INCLUDE=${INCLUDE}" "LIBDIR=${LIBDIR}" "DEPENDSFILE=../${DEPENDSFILE}" "BINDIR=../${BINDIR}" "LOCALDIR=${LOCALDIR}" "XLIBDIR=${XLIBDIR}" "GLLIBDIR=${GLLIBDIR}" "QTDIR=${QTDIR}" "LINKS=${LINKS}" "MOC=${MOC}" "MOCFILE=../${MOCFILE}" "INCDIR=${INCDIR}"

all: directories mocs server

directories:
	if [ ! -d $(OBJDIR) ] ; then mkdir $(OBJDIR) ; fi
	if [ ! -d $(BINDIR) ] ; then mkdir $(BINDIR) ; fi
	if [ ! -d $(LIBDIR) ] ; then mkdir $(LIBDIR) ; fi	
	touch $(MOCFILE)
	if [ ! -f $(DEPENDSFILE) ] ; \
	then touch $(DEPENDSFILE) ; make depends ; fi
	cd $(OBJDIR) ; ln -sf ../$(SRCDIR)/* .

mocs:
	cd $(OBJDIR); make $(OPTIONS) mocs

server:
	cd $(OBJDIR); make $(OPTIONS) all

depends:
	if [ ! -f $(DEPENDSFILE) ] ; \
	then touch $(DEPENDSFILE) ; fi
	cd $(SRCDIR); make $(OPTIONS) depends

pretty:
	find . \( -name 'core' -o -name '*~' \) -exec rm -f {} \;

clean:
	@make pretty
	cd $(OBJDIR); rm -f *.o

veryclean:
	@make pretty
	rm -rf $(OBJDIR)

# install
COPYFILES=bin/filewatcher
COPYDIRS=
COPYTREES=
DESTNAME=filewatcher-1.0.0

include ../conf/install.mk

