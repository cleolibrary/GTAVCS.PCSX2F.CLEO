EE_BIN = ../../data/PLUGINS/GTAVCS.PCSX2F.CLEO.elf
EE_OBJS = main.o ../../includes/pcsx2/log.o ../../includes/pcsx2/memalloc.o ../../includes/pcsx2/patterns.o ../../includes/pcsx2/injector.o ../../includes/pcsx2/rini.o ../../includes/pcsx2/inireader.o ../../includes/pcsx2/mips.o 

BASE_ADDRESS = 0x05000000
EE_LINKFILE = linkfile
EE_LIBS += -l:libc.a -l:libm.a -l:libgcc.a
EE_LDFLAGS = -Wl,--entry=init -Wl,-Map,../../data/PLUGINS/GTAVCS.PCSX2F.CLEO.map -nostdlib -nodefaultlibs -Wl,'--defsym=BASE_ADDRESS=$(BASE_ADDRESS)'
EE_CXXFLAGS += -fpermissive

all: clean main-build

main-build: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

PS2SDK = ../../external/ps2sdk/ps2sdk
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
