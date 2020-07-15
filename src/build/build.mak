###########################################################################
#
#   build.mak
#
#   MAME build tools makefile
#
#   Copyright Nicola Salmoria and the MAME Team.
#   Visit http://mamedev.org for licensing and usage restrictions.
#
###########################################################################

OBJDIRS += \
	$(BUILDOBJ) \



#-------------------------------------------------
# set of build targets
#-------------------------------------------------

FILE2STR = $(BUILDOUT)/file2str$(BUILD_EXE)
PNG2BDC = $(BUILDOUT)/png2bdc$(BUILD_EXE)
VERINFO = $(BUILDOUT)/verinfo$(BUILD_EXE)


BUILD += $(FILE2STR)
BUILD += $(PNG2BDC)
BUILD += $(VERINFO)


ifneq ($(CROSS_BUILD),1)

#-------------------------------------------------
# file2str
#-------------------------------------------------

FILE2STROBJS = $(BUILDOBJ)/file2str.o

$(FILE2STR): $(FILE2STROBJS) $(LIBOCORE)
	@echo Linking $@...
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@
	cp $@ $(BUILDSRC)/



#-------------------------------------------------
# png2bdc
#-------------------------------------------------

PNG2BDCOBJS = $(BUILDOBJ)/png2bdc.o 

$(PNG2BDC): $(PNG2BDCOBJS) $(LIBUTIL) $(LIBOCORE) $(ZLIB)
	@echo Linking $@...
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@
	cp $@ $(BUILDSRC)/



#-------------------------------------------------
# verinfo
#-------------------------------------------------

VERINFOOBJS = $(BUILDOBJ)/verinfo.o

$(VERINFO): $(VERINFOOBJS) $(LIBOCORE)
	@echo Linking $@...
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@
	cp $@ $(BUILDSRC)/

else



$(FILE2STR): $(BUILDSRC)/file2str
	@echo Copying $@...
	cp $< $@

$(PNG2BDC): $(BUILDSRC)/png2bdc
	@echo Linking $@...
	cp $< $@

$(VERINFO): $(BUILDSRC)/verinfo
	@echo Linking $@...
	cp $< $@

endif


