# build/os-auto.mak.  Generated from os-auto.mak.in by configure.

export OS_CFLAGS   := $(CC_DEF)PJ_AUTOCONF=1 -O2 -Wno-unused-label -DPJ_SDK_NAME="\"iPhoneOS5.1.sdk\"" -arch armv7 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.1.sdk -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1

export OS_CXXFLAGS := $(CC_DEF)PJ_AUTOCONF=1 -O2 -Wno-unused-label -DPJ_SDK_NAME="\"iPhoneOS5.1.sdk\"" -arch armv7 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.1.sdk 

export OS_LDFLAGS  := -O2 -arch armv7 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.1.sdk -framework AudioToolbox -framework Foundation -lbz2 -lz -lpthread  -framework CoreAudio -framework CoreFoundation -framework AudioToolbox -framework CFNetwork -framework UIKit -framework AVFoundation -framework UIKit -framework CoreGraphics -framework QuartzCore -framework CoreVideo -framework CoreMedia 

export OS_SOURCES  := 


