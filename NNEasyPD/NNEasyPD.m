//
//  NNEasyPD.m
//  NNEasyPD
//
//  Copyright © 2015 notnatural, LLC. All rights reserved.
//

#import "NNEasyPD.h"

@implementation NNEasyPD

- (instancetype)init
{
    self = [super init];
    if (self) {
        [self createDispatcher];
    }
    return self;
}

- (void)sendFloat:(float)f toReceiver:(NSString*)r {
    [PdBase sendFloat:f toReceiver:r];
}
- (void)sendBangToReceiver:(NSString*)r {
    [PdBase sendBangToReceiver:r];
}
- (void)sendSymbol:(NSString*)s toReceiver:(NSString*)r {
    [PdBase sendSymbol:s toReceiver:r];
}
- (void)sendMessage:(NSString*)s withArguments:(NSArray*)a toReceiver:(NSString*)r {
    [PdBase sendMessage:s withArguments:a toReceiver:r];
}

- (UIViewController *)currentTopViewController {
    UIViewController *topVC = [[[[UIApplication sharedApplication] delegate] window] rootViewController];
    while (topVC.presentedViewController) {
        topVC = topVC.presentedViewController;
    }
    return topVC;
}

- (void)simpleAlert:(NSString*)title message:(NSString*)message {
    UIAlertController * alert=   [UIAlertController
                                  alertControllerWithTitle:title
                                  message:message
                                  preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction* ok = [UIAlertAction
                         actionWithTitle:@"OK"
                         style:UIAlertActionStyleDefault
                         handler:^(UIAlertAction * action)
                         {
                             [alert dismissViewControllerAnimated:YES completion:nil];
                             
                         }];
    
    [alert addAction:ok];
    
    [[self currentTopViewController] presentViewController:alert animated:YES completion:nil];
}

- (void)createDispatcher {
    /* Account for the 48k native sampling rate on iPhone 6s */
    float sr = 44100;
    UIViewController *currentTopVC = [self currentTopViewController];
    if ([[currentTopVC.view traitCollection] forceTouchCapability] == UIForceTouchCapabilityAvailable) {
        sr = 48000;
    }
    
    [[NSNotificationCenter defaultCenter]
     addObserver:self
     selector:@selector(applicationWillTerminate:)
     name:UIApplicationWillTerminateNotification
     object:[UIApplication sharedApplication]];
    
    audioController = [[PdAudioController alloc] init];
    if ([audioController configureAmbientWithSampleRate:sr
                                              numberChannels:2
                                               mixingEnabled:YES] != PdAudioOK )
    {
        [self simpleAlert:@"Audio Error" message:@"Failed to initialize PD audio controller."];
    } else {
        [self setActive:YES];
        dispatcher = [[PdDispatcher alloc] init];
        [PdBase setDelegate:dispatcher];
    }
}

- (void)loadPatch:(NSString*)patchString {
    
    patch = [PdBase openFile:patchString path:[[NSBundle mainBundle] resourcePath]];
    if (!patch) {
        [self simpleAlert:@"PD Error" message:@"Failed to load patch."];
    }
}

- (void)unloadPatch {
    [PdBase closeFile:patch];
    patch = nil;
}

- (void)setActive:(BOOL)isActive {
    if (isActive == YES) {
        audioController.active = YES;
    } else {
        audioController.active = NO;
    }
}

- (void)resetPD {
    [self setActive:NO];
    [self unloadPatch];
    audioController = nil;
    dispatcher = nil;
    patch = nil;
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [self resetPD];
}

@end
