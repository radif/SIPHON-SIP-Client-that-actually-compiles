//
//  InAppSettingsReader.m
//  InAppSettingsTestApp
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 1/19/10.
//  Copyright 2010 InScopeApps{+}. All rights reserved.
//

#import "InAppSettingsReader.h"
#import "InAppSettingsSpecifier.h"
#import "InAppSettingsConstants.h"

@implementation InAppSettingsReaderRegisterDefaults

@synthesize files;
@synthesize values;

- (void)loadFile:(NSString *)file{
    //if the file is not in the files list we haven't read it yet
    NSInteger fileIndex = [self.files indexOfObject:file];
    if(fileIndex == NSNotFound){
        [self.files addObject:file];
        
        //load plist
        NSDictionary *settingsDictionary = [[NSDictionary alloc] initWithContentsOfFile:InAppSettingsFullPlistPath(file)];
        NSArray *preferenceSpecifiers = [settingsDictionary objectForKey:InAppSettingsPreferenceSpecifiers];
        NSString *stringsTable = [settingsDictionary objectForKey:InAppSettingsStringsTable];
        
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        for(NSDictionary *eachSetting in preferenceSpecifiers){
            InAppSettingsSpecifier *setting = [[InAppSettingsSpecifier alloc] initWithDictionary:eachSetting andStringsTable:stringsTable];
            if([setting isValid]){
                if([setting isType:InAppSettingsPSChildPaneSpecifier]){
                    [self loadFile:[setting valueForKey:InAppSettingsSpecifierFile]];
                }else if([setting hasKey]){
                    if([setting valueForKey:InAppSettingsSpecifierDefaultValue]){
                        [self.values 
                            setObject:[setting valueForKey:InAppSettingsSpecifierDefaultValue] 
                            forKey:[setting getKey]];
                    }
                }
            }
            [setting release];
        }
        [pool drain];
        [settingsDictionary release];
    }
}

- (id)initWithRootFile:(NSString *)rootFile{
	self = [super init];
	if (self != nil) {
		self.files = [[NSMutableArray alloc] init];
		self.values = [[NSMutableDictionary alloc] init];
		[self loadFile:rootFile];
		[[NSUserDefaults standardUserDefaults] registerDefaults:self.values];
	}
	return self;
}


- (id)init{
	return [self initWithRootFile:InAppSettingsRootFile];
}


- (void)dealloc{
    [files release];
    [values release];
    [super dealloc];
}

@end

@implementation InAppSettingsReader

@synthesize file;
@synthesize headers, settings, footers;
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0
@synthesize hasDynamicContents;
#endif /* DYNAMIC_CONTENT_CELLS */

- (id)initWithFile:(NSString *)inputFile{
    self = [super init];
    if(self != nil){
        self.file = inputFile;
        
        //load plist
        NSDictionary *settingsDictionary = [[NSDictionary alloc] initWithContentsOfFile:InAppSettingsFullPlistPath(self.file)];
        NSArray *preferenceSpecifiers = [settingsDictionary objectForKey:InAppSettingsPreferenceSpecifiers];
        NSString *stringsTable = [settingsDictionary objectForKey:InAppSettingsStringsTable];
        
        //initialize the arrays
        self.headers = [[NSMutableArray alloc] init];
        self.settings = [[NSMutableArray alloc] init];
				self.footers = [[NSMutableArray alloc] init];
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0			
				self.hasDynamicContents = [[NSMutableArray alloc] init];
#endif /* DYNAMIC_CONTENT_CELLS */			
				//initialize the object
				NSString *aClassName = [NSString stringWithFormat:@"%@Settings", 
																[inputFile stringByDeletingPathExtension]];
				Class aClass = NSClassFromString(aClassName);
				id anObject = [[aClass alloc] init];
        
        //load the data
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        for(NSDictionary *eachSetting in preferenceSpecifiers){
            InAppSettingsSpecifier *setting = [[InAppSettingsSpecifier alloc] initWithDictionary:eachSetting andStringsTable:stringsTable];
            if([setting isValid]){
                if([setting isType:InAppSettingsPSGroupSpecifier]){
                    [self.headers addObject:[setting localizedTitle]];
                    [self.settings addObject:[NSMutableArray array]];
										[self.footers addObject:[setting localizedFooter]];
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0										
										[self.hasDynamicContents addObject:[NSNumber numberWithBool:NO]];
#endif /* DYNAMIC_CONTENT_CELLS */									
                }else{
                    //if there are no settings make an initial container
                    if([self.settings count] < 1){
                        [self.headers addObject:@""];
                        [self.settings addObject:[NSMutableArray array]];
												[self.footers addObject:@""];
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0										
												[self.hasDynamicContents addObject:[NSNumber numberWithBool:NO]];
#endif /* DYNAMIC_CONTENT_CELLS */	
                    }
										[[self.settings lastObject] addObject:setting];
										setting.object = anObject;
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0
										if ([setting isType:InAppSettingsPSDynamicPaneSpecifier])
											[self.hasDynamicContents replaceObjectAtIndex:0
																												 withObject:[NSNumber numberWithBool:YES]];
#endif /* DYNAMIC_CONTENT_CELLS */
                }
            }
            [setting release];
        }
        [pool drain];
				[anObject release];
        [settingsDictionary release];
    }
    return self;
}

- (void)dealloc{
    [file release];
    [headers release];
    [settings release];
		[footers release];
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0			
		[hasDynamicContents release];
#endif /* DYNAMIC_CONTENT_CELLS */
    [super dealloc];
}

@end
