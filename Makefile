#
#
#

# Windows compiler under mac
		CCWIN64=x86_64-w64-mingw32-g++

# macOS compiler
CCOSX64=cc

all:	FSAvionicsBridge XPAvionicsBridgeMac XPAvionicsBridgeWin

# MSFS stuff

FSCFLAGS=-I. -I./FSSDK -m64 -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -D_WIN32=1
FSLFLAGS=-m64 -static -static-libgcc -static-libstdc++
FSLIBS=-lws2_32 ./FSSDK/SimConnect.lib

FSAvionicsBridge.o: FSAvionicsBridge.cpp FSFlightData.h Network.h Utilities.h
		$(CCWIN64) -o FSAvionicsBridge.o $(FSCFLAGS) -c FSAvionicsBridge.cpp

FSFlightData.o: FSFlightData.cpp FSFlightData.h Network.h Utilities.h
		$(CCWIN64) -o FSFlightData.o $(FSCFLAGS) -c FSFlightData.cpp

FSNetwork.o: Network.cpp Network.h
		$(CCWIN64) -o FSNetwork.o $(FSCFLAGS) -c Network.cpp

FSUtilities.o: Utilities.cpp Utilities.h
		$(CCWIN64) -o FSUtilities.o $(FSCFLAGS) -c Utilities.cpp

FSAvionicsBridge: FSAvionicsBridge.o FSFlightData.o FSNetwork.o FSUtilities.o
	$(CCWIN64) -o FSAvionicsBridge.exe $(FSLFLAGS) FSAvionicsBridge.o FSFlightData.o FSNetwork.o FSUtilities.o $(FSLIBS)

# X-Plane stuff

XPSRCS=Network.cpp Utilities.cpp XPAvionicsBridge.cpp
XPHDRS=Network.h Utilities.h

XPCFLAGS=-DXPLM200=1 -DXPLM210=1 -DXPL=1 -Wunused-function -Wunused-label -Wunused-value -Wunused-variable
XPCFLAGSOSX64X86=-m64 -arch x86_64 -DIBM=0 -DAPL=1 -DLIN=0
XPCFLAGSOSX64ARM=-m64 -arch arm64 -DIBM=0 -DAPL=1 -DLIN=0
XPINCLUDES=-I. -I./XPSDK/CHeaders/XPLM -I./XPSDK/CHeaders/Widgets
XPLFLAGSOSX64X86=-mmacosx-version-min=10.6 -m64 -arch x86_64 -flat_namespace -dynamiclib -shared -L./XPSDK/Libraries/Mac
XPLFLAGSOSX64ARM=-mmacosx-version-min=10.6 -m64 -arch arm64 -flat_namespace -dynamiclib -shared # -L./XPSDK/Libraries/Mac
XPLIBSOSX64=-F./XPSDK/Libraries/Mac -framework XPLM -framework XPWidgets # -framework XPLM # -framework XPWidgets
XPCFLAGSWIN64=-DIBM=1 -DAPL=0 -DLIN=0 -D_WIN32=1
XPLFLAGSWIN64=-m64 -shared -L ./XPSDK/Libraries/Win # -ggdb -flat_namespace
XPLIBSWIN64=-lXPLM_64 -lXPWidgets_64 -lws2_32	#-lwsock32

XPNetworkMac.o: Network.cpp Network.h

XPAvionicsBridgeMac: $(XPSRCS) $(XPHDRS)
	$(CCOSX64) -o mac-x86_64.xpl $(XPSRCS) $(XPCFLAGS) $(XPCFLAGSOSX64X86) $(XPINCLUDES) $(XPLFLAGSOSX64X86) $(XPLIBSOSX64)
	$(CCOSX64) -o mac-arm64.xpl $(XPSRCS) $(XPCFLAGS) $(XPCFLAGSOSX64ARM) $(XPINCLUDES) $(XPLFLAGSOSX64ARM) $(XPLIBSOSX64)
	lipo -create -output XPAvionicsBridgeMac.xpl  mac-arm64.xpl mac-x86_64.xpl
	rm -f mac-arm64.xpl mac-x86_64.xpl
	
XPAvionicsBridgeWin: $(XPSRCS) $(XPHDRS)
		$(CCWIN64) -o XPAvionicsBridgeWin.xpl $(XPSRCS) $(XPCFLAGS) $(XPCFLAGSWIN64) $(XPINCLUDES) $(XPLFLAGSWIN64) $(XPLIBSWIN64)

# Generic stuff

clean:
	rm -f *.o FSAvionicsBridge.exe XPAvionicsBridgeMac.xpl XPAvionicsBridgeWin.xpl
	
