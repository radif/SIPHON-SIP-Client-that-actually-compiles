/*
 *  AddressBook.h
 *  Siphon
 *
 *  Created by Samuel Vinson on 04/06/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


#ifdef __cplusplus
extern "C" {
#endif
	
#import "ABTypedefs.h"
#import "ABAddressBook.h"
//#import <AddressBook/ABRecord.h>
//#import <AddressBook/ABGroup.h>
#import "ABRecord.h"
#import "ABSearchElement.h"

#define ABAddressBook   SVAddressBook
#define ABRecord        SVRecord
#define ABPerson        SVRecord
#define ABGroup         SVRecord
#define ABSearchElement SVSearchElement
	
#ifdef __cplusplus
}
#endif