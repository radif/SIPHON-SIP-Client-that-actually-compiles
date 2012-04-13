CC=arm-apple-darwin-gcc
CXX=arm-apple-darwin-g++
LD=$(CC)

PRODUCT_NAME = Siphon
SETTINGS_NAME=$(PRODUCT_NAME)Settings
SETTINGS_BUNDLE=$(SETTINGS_NAME).bundle
BUILT_PRODUCTS_DIR = ../dist


LDFLAGS= -Wall -lobjc \
	-framework CoreFoundation \
	-framework Foundation \
	-framework Preferences \
	
CFLAGS= -O2

TARGET=$(BUILT_PRODUCTS_DIR)/$(SETTINGS_BUNDLE)/$(SETTINGS_NAME)
VERSION=1.9.9

SOURCES=\
	SiphonSettings.m


OBJECTS= \
	$(patsubst %.c,%.o,$(filter %.c,$(SOURCES))) \
	$(patsubst %.m,%.o,$(filter %.m,$(SOURCES)))

LANGUAGES= \
	$(wildcard *.lproj)
	
PLIST= \
	$(wildcard *.plist)

all:	$(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p $(BUILT_PRODUCTS_DIR)/$(SETTINGS_BUNDLE)
	$(LD) $(LDFLAGS) -bundle -o $@ $^
	cp Siphon.plist Advanced.plist $(BUILT_PRODUCTS_DIR)/$(SETTINGS_BUNDLE)/
	cp icon.png $(BUILT_PRODUCTS_DIR)/$(SETTINGS_BUNDLE)/
	for i in $(LANGUAGES); do \
		mkdir -p $(BUILT_PRODUCTS_DIR)/$(SETTINGS_BUNDLE)/$$i; \
		cp $$i/Localizable.strings $(BUILT_PRODUCTS_DIR)/$(SETTINGS_BUNDLE)/$$i; \
	done

%.o:	%.m
	$(CC) -Wall -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o:	%.c
	$(CC) -Wall -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)

