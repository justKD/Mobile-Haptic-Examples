//
//  FaustLoader.m
//  Mobile Haptic Examples
//
//  Copyright © 2015 notnatural, LLC. All rights reserved.
//

#import "FaustLoader.h"

extern "C" void pluck_harp10_tilde_setup(void);
extern "C" void pluck_one_string_pitch_control_auto_timbre_tilde_setup(void);
extern "C" void gts_tilde_setup(void);

@implementation FaustLoader

+ (void)setupHarp10 {
    pluck_harp10_tilde_setup();
}

+ (void)setupHarp01 {
    pluck_one_string_pitch_control_auto_timbre_tilde_setup();
}

+ (void)setupGts {
    gts_tilde_setup();
}

@end
