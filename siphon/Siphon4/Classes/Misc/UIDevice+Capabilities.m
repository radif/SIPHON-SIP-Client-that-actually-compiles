/**
 *  Siphon SIP-VoIP for iPhone and iPod Touch
 *  Copyright (C) 2011 Samuel <samuelv0304@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#import "UIDevice+Capabilities.h"

#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>
#import <AudioToolbox/AudioToolbox.h>

#define UIDeviceBluetoothCapability	@"bluetooth"
#define UIDeviceCellularDataCapability	@"cellular-data"
//#define UIDeviceDataPlanCapability	@"data-plan"
#define UIDeviceMicrophoneCapability	@"microphone"
#define UIDeviceMultitaskingCapability	@"multitasking"
#define UIDeviceTelephonyCapability	@"telephony"
//#define UIDeviceVeniceCapability	@"venice" // Video Conferencing
//#define UIDeviceVOIPCapability	@"voip"
#define UIDeviceWiFiCapability	@"wifi"


@implementation UIDevice (Capabilities)

/*- (BOOL) supportsCapability: (NSString *) capability
{
	
}*/

- (BOOL)supportsMicrophone
{
	UInt32 hasMicro, size;

  size = sizeof(hasMicro);
  AudioSessionGetProperty(kAudioSessionProperty_AudioInputAvailable,
                          &size, &hasMicro);
	
	return (hasMicro ? YES : NO);
}

- (BOOL)supportsMultitask
{
	UIDevice* device = [UIDevice currentDevice];
	BOOL backgroundSupported = NO;
	if ([device respondsToSelector:@selector(isMultitaskingSupported)])
		backgroundSupported = device.multitaskingSupported;
	return backgroundSupported;
}

- (BOOL)supportsTelephony
{
	UIDevice* device = [UIDevice currentDevice];
	BOOL telephony = [[device model] hasPrefix:@"iPhone"];
	
	if (telephony)
	{
		CTTelephonyNetworkInfo *networkInfo = [[[CTTelephonyNetworkInfo alloc] init] autorelease];
		CTCarrier *carrier = [networkInfo subscriberCellularProvider];
		telephony = ([carrier carrierName] != nil);
	}
	
	return telephony;
}



@end
