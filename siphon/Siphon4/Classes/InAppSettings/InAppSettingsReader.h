//
//  InAppSettingsReader.h
//  InAppSettingsTestApp
//
//  Modified by Samuel Vinson 2010-2011 - GPL
//  Created by David Keegan on 1/19/10.
//  Copyright 2010 InScopeApps{+}. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "InAppSettingsConstants.h"

@interface InAppSettingsReaderRegisterDefaults : NSObject {
    //keep track of what files we've read to avoid circular references
    NSMutableArray *files;
    NSMutableDictionary *values;
}

@property (nonatomic, retain) NSMutableArray *files;
@property (nonatomic, retain) NSMutableDictionary *values;

- (id)initWithRootFile:(NSString *)rootFile;

@end

@interface InAppSettingsReader : NSObject {
    NSString *file;
    NSMutableArray *headers, *settings, *footers;
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0
		NSMutableArray *hasDynamicContents;
#endif /* DYNAMIC_CONTENT_CELLS */
}

@property (nonatomic, copy) NSString *file;
@property (nonatomic, retain) NSMutableArray *headers, *settings, *footers;
#if defined(DYNAMIC_CONTENT_CELLS) && DYNAMIC_CONTENT_CELLS!=0
@property (nonatomic, retain) NSMutableArray *hasDynamicContents;
#endif /* DYNAMIC_CONTENT_CELLS */

- (id)initWithFile:(NSString *)inputFile;

@end
