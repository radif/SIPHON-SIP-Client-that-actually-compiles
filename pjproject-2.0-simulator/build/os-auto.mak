# build/os-auto.mak.  Generated from os-auto.mak.in by configure.

export OS_CFLAGS   := $(CC_DEF)PJ_AUTOCONF=1 -O2 -m32 -miphoneos-version-min=4.0 -DPJ_SDK_NAME="\"iPhoneSimulator5.1.sdk\"" -arch i686 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.1.sdk -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1

export OS_CXXFLAGS := $(CC_DEF)PJ_AUTOCONF=1 -O2 -m32 -miphoneos-version-min=4.0 -DPJ_SDK_NAME="\"iPhoneSimulator5.1.sdk\"" -arch i686 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.1.sdk 

export OS_LDFLAGS  := -O2 -arch i686 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.1.sdk -framework AudioToolbox -framework Foundation -framework CFNetwork -lpthread  -framework CoreAudio -framework AudioToolbox 

export OS_SOURCES  := 


