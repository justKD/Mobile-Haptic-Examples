//
//  NNEasyPD.h
//  NNEasyPD
//
//  Copyright © 2015 notnatural, LLC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#import "PdAudioController.h"
#import "PdDispatcher.h"

@interface NNEasyPD : UIControl <PdReceiverDelegate> {
    PdDispatcher *dispatcher;
    void *patch;
    PdAudioController *audioController;
}

- (void)loadPatch:(NSString*)patchString;
- (void)unloadPatch;
- (void)setActive:(BOOL)isActive;
- (void)resetPD;
- (void)sendFloat:(float)f toReceiver:(NSString*)receiver;
- (void)sendBangToReceiver:(NSString*)r;
- (void)sendSymbol:(NSString*)s toReceiver:(NSString*)r;
- (void)sendMessage:(NSString*)s withArguments:(NSArray*)a toReceiver:(NSString*)r;

@end

/*
 Instructions
 
 ADD LIBPD:
 
 - Copy the NNEasyPD folder (includes NNEasyPD class files and LIBPD folder) to your project folder.
 
 - Add the LIBPD Xcode project to your Xcode project.
	- Use “Add files to…”
	- The path is <NNEasyPD/libpd/libpd.xcodeproj>
 
 - Add the NNEasyPD class files to your Xcode project.
 
 - Go to “Build Phases” and add ‘libpd-ios’ to ‘Target Dependencies’.
 
 - Also in “Build Phases” add ‘libpd-ios.a’ to ‘Link Binary With Libraries’.
	- ‘libpd-ios.a’ may show up red: do not worry about it.
 
 - Now go to “Build Settings” and search for “User Header Search Paths”
	- Double click on the field to add a search path
	- Add the path “nneasypd/libpd/..” (without quotes) and set it to “recursive”
 
 
 Set up NNEasyPD:
 
 - Use “Add files to…” and add your PD file.
	- Notes on creating a PD file to use with LIBPD are below.
 
 - Import “NNEasyPD.h” to your .m file
 
 - Add a property for NNEasyPD
	- e.g.- @property (nonatomic, strong) NNEasyPD *pd;
 
 - Finish set up in an appropriate place, such as the viewDidLoad: method of a view controller
	- Example set up code:
 
 self.pd = [[NNEasyPD alloc] init];
 NSString *patchString = [NSString stringWithFormat:@"NNEasyPD_Demo.pd"];
 [self.pd initializeWithPatch:patchString];
 
 
 Use NNEasyPD:
 
 - Your PD patch should have typical PD receive objects set up to receive data from your iOS project
	- For example, the frequency inlet for an [osc~] object might be connected to an [r freq] object.
 - The string for that receiver would simply be @"freq"
 
 - In order to send data to PD, call methods as follows:
 
	- Example method calls based on the above example set up code:
 - These calls are identical to the normal LIBPD ones. The only difference is that all of the PDBase set up code is wrapped into its own Cocoa Touch class.
	
 [self.pd sendBangToReceiver:@"click"];
 [self.pd sendFloat:sender.value toReceiver:@"freq"];
 [self.pd sendFloat:1 toReceiver:@"start"];
 [self.pd sendSymbol:@“doit 1“ toReceiver:@“route”];
 
	- NNEasyPD defaults to audio on, but you can manually toggle audio processing with:
 
 [self.pd setActive:YES];
 [self.pd setActive:NO];
 
*/
