CC=arm-apple-darwin-gcc
CXX=arm-apple-darwin-g++
LD=$(CC)

TARGET=Siphon
VERSION=1.9.9

PRODUCT_NAME = Siphon
PRODUCT_APP=$(PRODUCT_NAME).app
BUILT_PRODUCTS_DIR = ../dist

APP_ABS=$(BUILT_PRODUCTS_DIR)/$(PRODUCT_APP)

LDFLAGS= -Wall \
	-framework CoreFoundation \
	-framework Foundation \
	-framework UIKit \
	-framework LayerKit \
	-framework GraphicsServices \
	-framework WebCore \
	-framework CoreGraphics \
	-framework AudioToolbox  \
	-framework iTunesStore \
	-framework Message  \
	-framework Celestial \
	-framework MobileMusicPlayer \
	-framework WebKit \
	-framework AddressBookUI \
	-framework AddressBook \
	-framework TelephonyUI \
	-lobjc \
	-lssl -lcrypto \
	-L../pjsip/lib \
	-lsrtp-arm-apple-darwin \
	-lpjsua-arm-apple-darwin \
	-lpjsip-simple-arm-apple-darwin \
	-lpjsip-ua-arm-apple-darwin \
	-lpjsip-arm-apple-darwin \
	-lpjmedia-arm-apple-darwin \
	-lpjnath-arm-apple-darwin \
	-lpjlib-util-arm-apple-darwin \
	-lpj-arm-apple-darwin

#	-lresample-arm-apple-darwin	

CFLAGS= -O2 \
	-I../pjsip/include \
	-DPJ_DARWINOS=1 \
	-DVERSION_STRING="@\"$(PRODUCT_NAME) $(VERSION)\""

YOUTUBE_RESOURCES = \
	History.png HistorySelected.png \
	More.png MoreSelected.png \
	MostViewed.png MostViewedSelected.png \
	TopRated.png TopRatedSelected.png \
	Featured.png FeaturedSelected.png

RESOURCES = Default.png icon.png Dial.png DialSelected.png \
	dialerkeypad.png dialerkeypad_pressed.png \
	lcd_top.png \
	addcontact.png addcontact_pressed.png \
	call.png \
	delete.png delete_pressed.png \
	settings.png index.html

INFOPLIST_FILE=Info.plist

LANGUAGES=English.lproj French.lproj German.lproj Spanish.lproj

STATUS_ICONS = Default_Siphon.png FSO_Siphon.png

SOURCES=\
	main.m \
	Siphon.m \
	PhoneView.m \
	ContactView.m \
	CallView.m \
	FavoritesView.m \
	AboutView.m \
	DialerPhonePad.m \
	NSNotificationAdditions.m \
	ring.c \
	call.m \
	dtmf.c


OBJECTS= \
	$(patsubst %.c,%.o,$(filter %.c,$(SOURCES))) \
	$(patsubst %.m,%.o,$(filter %.m,$(SOURCES)))

all:	$(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p $(APP_ABS)
	$(LD) $(LDFLAGS) -o $(APP_ABS)/$@ $^

	for i in $(RESOURCES) $(INFOPLIST_FILE); do \
		cp app/$$i $(APP_ABS)/$$i; \
	done
	
	for i in $(YOUTUBE_RESOURCES); do \
		ln -fs /Applications/YouTube.app/$$i $(APP_ABS)/$$i; \
	done
	for i in $(LANGUAGES); do \
		mkdir -p $(APP_ABS)/$$i; \
		cp app/$$i/Localizable.strings $(APP_ABS)/$$i; \
	done

	mkdir -p $(BUILT_PRODUCTS_DIR)/SpringBoard.app
	for i in $(STATUS_ICONS); do \
		cp app/$$i $(BUILT_PRODUCTS_DIR)/SpringBoard.app/$$i; \
	done

%.o:	%.m
	$(CC) -Wall -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o:	%.c
	$(CC) -Wall -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf *.o *~ $(APP_ABS) $(BUILT_PRODUCTS_DIR)/SpringBoard.app

