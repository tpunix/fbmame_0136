###########################################################################
#
#   tiny.mak
#
#   Small driver-specific example makefile
#	Use make SUBTARGET=tiny to build
#
#   Copyright Nicola Salmoria and the MAME Team.
#   Visit  http://mamedev.org for licensing and usage restrictions.
#
###########################################################################

MAMESRC = $(SRC)/mame
MAMEOBJ = $(OBJ)/mame

AUDIO = $(MAMEOBJ)/audio
DRIVERS = $(MAMEOBJ)/drivers
LAYOUT = $(MAMEOBJ)/layout
MACHINE = $(MAMEOBJ)/machine
VIDEO = $(MAMEOBJ)/video

OBJDIRS += \
	$(AUDIO) \
	$(DRIVERS) \
	$(LAYOUT) \
	$(MACHINE) \
	$(VIDEO) \



#-------------------------------------------------
# Specify all the CPU cores necessary for the
# drivers referenced in tiny.c.
#-------------------------------------------------

CPUS += Z80
CPUS += MIPS
CPUS += RSP
CPUS += V60
CPUS += M680X0
CPUS += SH2
CPUS += V810
CPUS += UPD7725
CPUS +=

#-------------------------------------------------
# Specify all the sound cores necessary for the
# drivers referenced in tiny.c.
#-------------------------------------------------

SOUNDS += SAMPLES
SOUNDS += DMADAC
SOUNDS += YMF271
SOUNDS += YMZ280B
SOUNDS += OKIM6295
SOUNDS += AY8910
SOUNDS += MSM5205
SOUNDS += YM2413
SOUNDS += YM2610
SOUNDS += YM2610B
SOUNDS += YM2203
SOUNDS += YM2608
SOUNDS += YMF278B
SOUNDS += ST0016
SOUNDS += NILE
SOUNDS += ES5506


#-------------------------------------------------
# This is the list of files that are necessary
# for building all of the drivers referenced
# in tiny.c
#-------------------------------------------------


DRVLIBS += $(DRIVERS)/bnstars.o
DRVLIBS += $(DRIVERS)/ms32.o $(VIDEO)/ms32.o $(DRIVERS)/tetrisp2.o $(VIDEO)/tetrisp2.o
DRVLIBS += $(DRIVERS)/jalmah.o
DRVLIBS += $(DRIVERS)/psikyo4.o $(VIDEO)/psikyo4.o
DRVLIBS += $(DRIVERS)/fromanc2.o $(VIDEO)/fromanc2.o
DRVLIBS += $(DRIVERS)/fromance.o $(VIDEO)/fromance.o

DRVLIBS += $(DRIVERS)/srmp2.o $(VIDEO)/srmp2.o
DRVLIBS += $(DRIVERS)/srmp5.o $(DRIVERS)/srmp6.o $(DRIVERS)/st0016.o $(DRIVERS)/speglsht.o $(VIDEO)/st0016.o
DRVLIBS += $(DRIVERS)/ssv.o $(VIDEO)/ssv.o


#-------------------------------------------------
# layout dependencies
#-------------------------------------------------

$(DRIVERS)/tetrisp2.o:  $(LAYOUT)/rocknms.lh

