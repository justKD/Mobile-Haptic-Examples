//
//  PluckViewController.m
//  Mobile Haptic Examples
//
//  Copyright © 2015 notnatural, LLC. All rights reserved.
//

#import "PluckViewController.h"
#import "nnKit.h"
#import "NNEasyPD.h"
#import "NNStringView.h"
#import "FaustLoader.h"
#import "DFContinuousForceTouchGestureRecognizer.h"

#define DEGREES_TO_RADIANS(angle) (angle/180.0*M_PI)

@interface PluckViewController () <DFContinuousForceTouchDelegate, UIGestureRecognizerDelegate>
{
    NNEasyPD *pd;
    NNStringView *stringView;
    UIView *menuView;
    
    CGFloat rotation;
    CGFloat lastRot;
    
    UILabel *value1;
    UILabel *value2;
    UILabel *value3;
    UILabel *value4;
    UILabel *value5;
    UILabel *value6;
    UILabel *value7;
    UILabel *value8;
    UILabel *value9;
    UILabel *value10;
    UILabel *value11;
    
    UIView *guideLine;
    UIView *pointer;
    
    CGFloat knobSize;
    BOOL hasDial;
    UIView *knob;
    UIView *stringLine;
    UIView *needle;
    
    BOOL touched;
    CGFloat damping;
}

@end

@implementation PluckViewController

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer{
    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer{
    return YES;
}
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch{
    return YES;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}
-(BOOL)shouldAutorotate {
    return NO;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupPD];
    [self createStringView];
    [self createMenu];
    
    damping = 0.3;
}

- (void)setupPD {
    pd = [[NNEasyPD alloc] init];
    NSString *patchString;
    switch (self.patchValue) {
        case 0:
            [FaustLoader setupHarp10];
            patchString = @"pluck_harp10-FireFaderSimulator-noiemlib.pd";
            break;
        case 1:
            [FaustLoader setupHarp01];
            patchString = @"pluck_one_string_pitch_control_auto_timbre-FireFaderSimulator.pd";
            break;
            
        default:
            break;
    }

    [pd loadPatch:patchString];
}

- (void)createStringView {
    stringView = [[NNStringView alloc] initWithFrame:CGRectMake(0, 50, SW()*.95, SH()-100)];
    stringView.center = self.view.center;
    stringView.backgroundColor = UIColorFromHex(0xD4D4D4);
    stringView.layer.cornerRadius = 20;
    stringView.layer.masksToBounds = YES;
    
//    pluck(k,R,maxdisp,-0.04*moveStrings),p0,junct0,dev0;
//    pluck(k,R,maxdisp,-0.032*moveStrings),p3,junct3,dev0;
//    pluck(k,R,maxdisp,-0.024*moveStrings),p5,junct5,dev0;
//    pluck(k,R,maxdisp,-0.016*moveStrings),p7,junct7,dev0;
//    pluck(k,R,maxdisp,-0.004*moveStrings),p9,junct9,dev0;
//    
//    pluck(k,R,maxdisp,0.004*moveStrings),p11,junct11,dev0;
//    pluck(k,R,maxdisp,0.012*moveStrings),p13,junct13,dev0;
//    pluck(k,R,maxdisp,0.020*moveStrings),p15,junct15,dev0;
//    pluck(k,R,maxdisp,0.028*moveStrings),p17,junct17,dev0;
//    pluck(k,R,maxdisp,0.036*moveStrings),p19,junct19,dev0;
    CGFloat add = 0.05;
    CGFloat mult = 10;
    NSArray *coefficients = @[[NSNumber numberWithFloat:-0.04],
                              [NSNumber numberWithFloat:-0.032],
                              [NSNumber numberWithFloat:-0.024],
                              [NSNumber numberWithFloat:-0.016],
                              [NSNumber numberWithFloat:-0.004],
                              [NSNumber numberWithFloat:0.004],
                              [NSNumber numberWithFloat:0.012],
                              [NSNumber numberWithFloat:0.020],
                              [NSNumber numberWithFloat:0.028],
                              [NSNumber numberWithFloat:0.036]
                              ];
    
    NSMutableArray *positions = [[NSMutableArray alloc] init];
    for (NSNumber *num in coefficients) {
        CGFloat value = ([num floatValue]  + add) * mult;
        [positions addObject:[NSNumber numberWithFloat:value]];
    }
    //NSLog(@"%@",positions);
    
    switch (self.patchValue) {
        case 0:
            stringView.stringColor = UIColorFromHex(0x3E4651);
            stringView.numberOfStrings = 10;
            stringView.stringPositions = positions;
            break;
        case 1:
            stringView.numberOfStrings = 0;
            break;
        default:
            break;
    }
    
    [self.view addSubview:stringView];
    
    UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [backButton addTarget:self
               action:@selector(goBack:)
     forControlEvents:UIControlEventTouchUpInside];
    [backButton setTitle:@"Back" forState:UIControlStateNormal];
    backButton.frame = CGRectMake(10, 10, 80, 40);
    //backButton.backgroundColor = [UIColor blueColor];
    [self.view addSubview:backButton];
    
    UIButton *menuButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [menuButton addTarget:self
               action:@selector(showMenu:)
     forControlEvents:UIControlEventTouchUpInside];
    [menuButton setTitle:@"Menu" forState:UIControlStateNormal];
    menuButton.frame = CGRectMake(10, self.view.frame.size.height-45, 80, 40);
    //menuButton.backgroundColor = [UIColor blueColor];
    [self.view addSubview:menuButton];
    
    if (self.patchValue == 0) {
        guideLine = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 6, stringView.frame.size.height*.95)];
        //guideLine.center = CGPointMake(stringView.frame.origin.x+10, SH()/2);
        guideLine.center = CGPointMake(stringView.frame.size.width-10, SH()/2);
        guideLine.layer.masksToBounds = YES;
        guideLine.layer.cornerRadius = 2;
        guideLine.backgroundColor = [UIColor blackColor];
        [self.view addSubview:guideLine];
        
//        pointer = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 10, 10)];
//        pointer.center = CGPointMake(stringView.frame.origin.x+10, SH()/2);
//        pointer.layer.masksToBounds = YES;
//        pointer.layer.cornerRadius = 6;
//        pointer.backgroundColor = UIColorFromHex(0xF1654C);
//        [self.view addSubview:pointer];
    } else {
        UIRotationGestureRecognizer *rot = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRot:)];
        rot.delegate = self;
        [stringView addGestureRecognizer:rot];
        
        DFContinuousForceTouchGestureRecognizer *ft = [[DFContinuousForceTouchGestureRecognizer alloc] init];
        ft.forceTouchDelegate = self;
        [self.view addGestureRecognizer:ft];
        
        hasDial = YES;
    }
}

- (void)handleRot:(UIRotationGestureRecognizer*)sender {
    //NSLog(@"%f",sender.rotation);
    //NSLog(@"velocity %f",sender.velocity);
    
    CGPoint center = [sender locationInView:sender.view];
    CGFloat stringThickness = 5;
    //NSLog(@"%f %f",center.x,center.y);
    
    if (sender.state == UIGestureRecognizerStateBegan) {
        //initCenter = center;
        
        knobSize = 220;
        knob = [[UIView alloc] initWithFrame:CGRectMake(0, 0, knobSize, knobSize)];
        knob.center = center;
        knob.layer.masksToBounds = YES;
        knob.layer.cornerRadius = knobSize/2;
        knob.backgroundColor = UIColorFromHex(0xF1654C);
        
        [nnKit animateViewGrowAndShow:knob or:nil completion:nil];
        [self.view addSubview:knob];
        
        stringLine = [[UIView alloc] initWithFrame:CGRectMake(0, 0, stringView.frame.size.width, stringThickness)];
        stringLine.center = CGPointMake(SW()/2, center.y);
        stringLine.backgroundColor = UIColorFromHex(0x3E4651);
        
        [nnKit animateViewGrowAndShow:stringLine or:nil completion:nil];
        [self.view addSubview:stringLine];
        
        CGFloat needleSize = 20;
        needle = [[UIView alloc] initWithFrame:CGRectMake(0, 0, needleSize, needleSize)];
        CGFloat degrees = lastRot * 360;
        degrees = DEGREES_TO_RADIANS(degrees);
        [needle setCenter:[self pointAroundCircumferenceFromCenter:knob.center withRadius:knobSize/2 andAngle:degrees]];
        needle.layer.masksToBounds = YES;
        needle.layer.cornerRadius = needleSize/2;
        needle.backgroundColor = UIColorFromHex(0x3E4651);
        
        [nnKit animateViewGrowAndShow:needle or:nil completion:nil];
        [self.view addSubview:needle];
    }
    
    CGFloat diff = sender.rotation - lastRot;
    
    rotation = diff + rotation;
    rotation = MIN(1, rotation);
    rotation = MAX(0, rotation);
    
    CGFloat rota = 1 - rotation;
    
    CGFloat pos = rota * .1;
    pos = pos - .05;
    [pd sendFloat:pos toReceiver:@"firefader"];
    //NSLog(@"%f",pos);
    
    if (hasDial) {
        
        CGFloat min;
        CGFloat max;
        CGFloat x = center.x/stringView.frame.size.width;
        min = 25.;
        max = 100.;
        x = x * (max-min);
        x = x + min;
        [pd sendFloat:x toReceiver:@"brightness"];
        
        CGFloat y = center.y/stringView.frame.size.height;
        min = 20.;
        max = 30.;
        y = y * (max-min);
        y = y + min;
        //NSLog(@"%f",y);
        [pd sendFloat:y toReceiver:@"frequency"];
    
        // animate rotating needle around the knob center
        CGFloat degrees = rotation * 360;
        degrees = DEGREES_TO_RADIANS(degrees);
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [UIView animateWithDuration:0.001f
                                  delay:0.0f
                                options:UIViewAnimationOptionCurveLinear
                             animations:^{
                                 [needle setCenter:[self pointAroundCircumferenceFromCenter:knob.center withRadius:knobSize/2 andAngle:degrees]];
                             }
                             completion:^(BOOL finished) {
                             }];
        });
        
    } else {
        [self handlePointerPositionWithY:rota];
    }
    
    if (sender.state == UIGestureRecognizerStateEnded || sender.state == UIGestureRecognizerStateCancelled) {
        lastRot = 0;
        
        [nnKit animateViewShrinkAndWink:knob or:nil andRemoveFromSuperview:YES completion:nil];
        [nnKit animateViewShrinkAndWink:stringLine or:nil andRemoveFromSuperview:YES completion:nil];
        [nnKit animateViewShrinkAndWink:needle or:nil andRemoveFromSuperview:YES completion:nil];
        knob = nil;
        stringLine = nil;
        needle = nil;
    } else {
        lastRot = sender.rotation;
        
        knob.center = center;
        stringLine.frame = CGRectMake(center.x-stringView.frame.size.width, center.y, stringView.frame.size.width, stringThickness);
        //stringLine.center = CGPointMake(SW()/2, center.y);
    }
}

- (CGPoint)pointAroundCircumferenceFromCenter:(CGPoint)center withRadius:(CGFloat)radius andAngle:(CGFloat)theta {
    CGPoint point = CGPointZero;
    point.x = center.x + radius * cosf(theta);
    point.y = center.y + radius * sinf(theta);
    
    return point;
}

- (void)createMenu {
    menuView = [[UIView alloc] initWithFrame:CGRectMake(self.view.frame.size.width, self.view.frame.origin.y, self.view.frame.size.width, self.view.frame.size.height)];
    menuView.backgroundColor = [UIColor whiteColor];
    
    CGFloat yOrigin = 50;
    CGFloat yOffset = 12;
    CGFloat min = 0;
    CGFloat max = 1;
    CGFloat val = .5;
    NSString *title = @"unknown";
    
    for (int i = 1; i <= 10; i++) {
        CGFloat y = i * yOrigin;
        switch (i) {
            case 1:
                min = .5;
                max = 7.;
                //val = 2.5;
                switch (self.patchValue) {
                    case 0:
                        val = 2.5;
                        break;
                    case 1:
                        val = 7.;
                        break;
                    default:
                        break;
                }
                title = @"Bridge Attentuation";
                value1 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                value1.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value1];
                break;
            case 2:
                min = 10.;
                max = 100.;
                val = 100.;
                title = @"Brightness";
                value2 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                value2.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value2];
                break;
            case 3:
                min = .5;
                max = 7.0;
                //val = 2.0;
                switch (self.patchValue) {
                    case 0:
                        val = 2.;
                        break;
                    case 1:
                        val = 7.;
                        break;
                    default:
                        break;
                }
                title = @"Fret Attentuation";
                value3 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                value3.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value3];
                break;
            case 4:
                min = 0.;
                max = 2.5;
                
                switch (self.patchValue) {
                    case 0:
                        val = 0.7;
                        break;
                    case 1:
                        val = .7;
                        break;
                    default:
                        break;
                }
                
                title = @"Pluck Damping";
                value4 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                value4.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value4];
                break;
            case 5:
                min = .0001;
                max = .01;
                //val = .0005;
                switch (self.patchValue) {
                    case 0:
                        val = .0005;
                        break;
                    case 1:
                        val = .005;
                        break;
                    default:
                        break;
                }
                title = @"Pluck Half-width";
                value5 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.4f",val]];
                value5.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value5];
                break;
            case 6:
                min = .01;
                max = .99;
                val = .4;
                title = @"Pluck Position";
                value6 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                value6.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value6];
                break;
            case 7:
                min = .25;
                max = 75.;
                switch (self.patchValue) {
                    case 0:
                        val = 20.;
                        break;
                    case 1:
                        val = 1.7;
                        break;
                    default:
                        break;
                }
                title = @"String Wave Impedance";
                value7 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                value7.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value7];
                break;
            case 8:
                min = .01;
                max = 1.;
                val = .07; //.22;
                title = @"Z-Volume";
                value8 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                value8.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value8];
                break;
            case 9:
                switch (self.patchValue) {
                    case 0:
                        min = .1;
                        max = 1.4;
                        val = 1.;
                        title = @"Waveguides Pos Adj";
                        value9 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                        break;
                    case 1:
                        min = 20.;
                        max = 84.;
                        val = 20.;
                        title = @"Frequency";
                        value9 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                        break;
                        
                    default:
                        break;
                }
                value9.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value9];
                break;
            case 10:
                min = 0.01;
                max = 1000.;
                title = @"Volume";
                switch (self.patchValue) {
                    case 0:
                        val = 700.;
                        value10 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                        break;
                    case 1:
                        val = .01; //5.;
                        value10 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
                        break;
                        
                    default:
                        break;
                }
                value10.textAlignment = NSTextAlignmentRight;
                [menuView addSubview:value10];
                break;
//            case 11:
//                min = 0.;
//                max = 2000.;
//                title = @"Stiffness";
//                switch (self.patchValue) {
//                    case 0:
//                        val = 1000.;
//                        value11 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
//                        break;
//                    case 1:
//                        val = 1000.;
//                        value11 = [self createLabelWithyOrigin:y+yOffset xOrigin:menuView.bounds.size.width/2 title:[NSString stringWithFormat:@"%.2f",val]];
//                        break;
//                        
//                    default:
//                        break;
//                }
//                value11.textAlignment = NSTextAlignmentRight;
//                [menuView addSubview:value11];
//                break;
            default:
                break;
        }
        
        UISlider *slider = [self createSliderAtOrigin:y min:min max:max value:val];
        UILabel *label = [self createLabelWithyOrigin:y+yOffset xOrigin:10 title:title];
        slider.tag = i;
        [menuView addSubview:slider];
        [menuView addSubview:label];
        
        [self changeSettings:i value:val];
        //[pd sendFloat:0 toReceiver:@"stiffness"];
    }
    
    UIButton *menuButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [menuButton addTarget:self
                   action:@selector(hideMenu:)
         forControlEvents:UIControlEventTouchUpInside];
    [menuButton setTitle:@"Back" forState:UIControlStateNormal];
    menuButton.frame = CGRectMake(10, self.view.frame.size.height-40, 80, 40);
    [menuButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [menuView addSubview:menuButton];
    
    [menuView setUserInteractionEnabled:NO];
    [self.view addSubview:menuView];
}

- (void)showMenu:(UIButton*)sender {
    [stringView setUserInteractionEnabled:NO];
    [menuView setUserInteractionEnabled:YES];
    [UIView animateWithDuration:.2
                          delay:.1
                        options: UIViewAnimationOptionCurveEaseInOut
                     animations:^{
                         menuView.transform = CGAffineTransformTranslate(CGAffineTransformIdentity, -menuView.frame.size.width, 0);
                         //menuView.alpha = 1;
                     } completion:nil
     ];
}
- (void)hideMenu:(UIButton*)sender {
    [stringView setUserInteractionEnabled:YES];
    [menuView setUserInteractionEnabled:NO];
    [UIView animateWithDuration:.2
                          delay:.1
                        options: UIViewAnimationOptionCurveEaseInOut
                     animations:^{
                         menuView.transform = CGAffineTransformTranslate(CGAffineTransformIdentity, menuView.frame.size.width, 0);
                         //menuView.alpha = 1;
                     } completion:nil
     ];
}

- (UISlider*)createSliderAtOrigin:(CGFloat)yOrigin min:(CGFloat)min max:(CGFloat)max value:(CGFloat)val {
    CGRect frame = CGRectMake(30, yOrigin, self.view.frame.size.width-60, 10);
    UISlider *slider = [[UISlider alloc] initWithFrame:frame];
    [slider addTarget:self action:@selector(handleSliders:) forControlEvents:UIControlEventValueChanged];
    [slider setBackgroundColor:[UIColor clearColor]];
    slider.minimumValue = min;
    slider.maximumValue = max;
    slider.continuous = YES;
    slider.value = val;
    
    return slider;
}
- (UILabel*)createLabelWithyOrigin:(CGFloat)y xOrigin:(CGFloat)x title:(NSString*)title {
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(x, y, self.view.frame.size.width/2, 20)];
    label.text = title;
    
    return label;
}

- (void)handleSliders:(UISlider*)sender {
    [self changeSettings:(int)sender.tag value:sender.value];
}
- (void)changeSettings:(int)tag value:(CGFloat)value {
    switch (tag) {
        case 1:
            [pd sendFloat:value toReceiver:@"bridge"];
            value1.text = [NSString stringWithFormat:@"%.2f",value];
            break;
        case 2:
            [pd sendFloat:value toReceiver:@"brightness"];
            value2.text = [NSString stringWithFormat:@"%.2f",value];
            break;
        case 3:
            [pd sendFloat:value toReceiver:@"fret"];
            value3.text = [NSString stringWithFormat:@"%.2f",value];
            break;
        case 4:
            damping = value;
            [pd sendFloat:value toReceiver:@"damping"];
            value4.text = [NSString stringWithFormat:@"%.2f",value];
            break;
        case 5:
            [pd sendFloat:value toReceiver:@"halfwidth"];
            value5.text = [NSString stringWithFormat:@"%.4f",value];
            break;
        case 6:
            [pd sendFloat:value toReceiver:@"position"];
            value6.text = [NSString stringWithFormat:@"%.2f",value];
            break;
        case 7:
            [pd sendFloat:value toReceiver:@"impedance"];
            value7.text = [NSString stringWithFormat:@"%.2f",value];
            break;
        case 8:
            [pd sendFloat:value toReceiver:@"zvolume"];
            value8.text = [NSString stringWithFormat:@"%.2f",value];
            break;
        case 9:
            if (self.patchValue == 0) {
                [pd sendFloat:value toReceiver:@"adjustment"];
            } else {
                [pd sendFloat:value toReceiver:@"frequency"];
            }
            value9.text = [NSString stringWithFormat:@"%.2f",value];
            break;
        case 10:
            [pd sendFloat:value toReceiver:@"volume"];
            value10.text = [NSString stringWithFormat:@"%.2f",value];
            break;
//        case 11:
//            [pd sendFloat:value toReceiver:@"stiffness"];
//            value11.text = [NSString stringWithFormat:@"%.2f",value];
//            break;
            
        default:
            break;
    }
}

- (void)goBack:(UIButton*)sender {
    [pd resetPD];
    pd = nil;
    
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [[event allTouches] anyObject]; //[[event allTouches] anyObject];
    //NSLog(@"start");
    touched = YES;
    [self handleTouchPosition:touch];
    //[self handleTouchForce:touch];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [[event allTouches] anyObject]; //[[touches allObjects] objectAtIndex:0];
    
   // NSLog(@"move");
    [self handleTouchPosition:touch];
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, .03 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [self handleTouchForce:touch];
    });
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    //NSLog(@"end");
    touched = NO;
    [pd sendFloat:0.0 toReceiver:@"stiffness"];
    [pd sendFloat:0.0 toReceiver:@"damping"];
    //[pd sendFloat:0 toReceiver:@"onoff"];
}

- (void)handleTouchPosition:(UITouch*)touch {
    [pd sendFloat:1 toReceiver:@"onoff"];
    
    CGPoint loc = [touch locationInView:stringView];
    CGFloat min;
    CGFloat max;
    
    if (!hasDial) {
        if (stringView.userInteractionEnabled) {
            CGFloat y = [self scaleY:loc.y];
            //NSLog(@"%f",y);
            [pd sendFloat:y toReceiver:@"firefader"];
            
            // scale x to be between .1 and .9
            CGFloat x = loc.x/stringView.frame.size.width;
            min = .1;
            max = .9;
            x = x * (max-min);
            x = x + min;
            [pd sendFloat:x toReceiver:@"position"];
            
            x = loc.x/stringView.frame.size.width;
            min = min * 40.;
            max = (max + .1) * 100.;
            x = x * (max-min);
            x = x + min;
            //x = 100 - x;
            x = MIN(100, x);
            x = MAX(10, x);
            //NSLog(@"%f",x);
            [pd sendFloat:x toReceiver:@"brightness"];
            
            y = y * 10;
            y = y + .5;
            [self handlePointerPositionWithY:y];
        }
        //NSLog(@"Y: %f",y);
    } else {
        
        //[self handleTouchForce:touch];
    }
}

- (void)handlePointerPositionWithY:(CGFloat)y {
    dispatch_async(dispatch_get_main_queue(), ^{
        [UIView animateWithDuration:0.001f
                              delay:0.0f
                            options:UIViewAnimationOptionCurveLinear
                         animations:^{
                             pointer.center = CGPointMake(pointer.center.x, guideLine.frame.origin.y+(guideLine.frame.size.height*y));
                         }
                         completion:^(BOOL finished) {
                         }];
    });
}

- (void)handleTouchForce:(UITouch*)touch {
    if (!hasDial) {
        if (touched) {
            CGFloat force = touch.force/touch.maximumPossibleForce;
            force = force * 2000.;
            //NSLog(@"Force: %f",force);
            [pd sendFloat:force toReceiver:@"stiffness"];
            [pd sendFloat:damping toReceiver:@"damping"];
        } else {
            [pd sendFloat:0 toReceiver:@"damping"];
            [pd sendFloat:0 toReceiver:@"stiffness"];
        }
    }
}

- (CGFloat)scaleY:(CGFloat)y {
    y = y / stringView.frame.size.height;
    NSString *round = [NSString stringWithFormat:@"%.3f",y];
    y = [round doubleValue];
    y = y * 100.;
    y = y - 50.;
    y = y *.001;
    y = MIN(y, .05);
    y = MAX(-.05, y);
    
    return y;
}

#pragma DFContinuousForceTouchDelegate

- (void) forceTouchRecognized:(DFContinuousForceTouchGestureRecognizer*)recognizer {
    //NSLog(@"d");
}
- (void) forceTouchRecognizer:(DFContinuousForceTouchGestureRecognizer*)recognizer didStartWithForce:(CGFloat)force maxForce:(CGFloat)maxForce {
    [self handleFT:force maxForce:maxForce];
}
- (void) forceTouchRecognizer:(DFContinuousForceTouchGestureRecognizer*)recognizer didMoveWithForce:(CGFloat)force maxForce:(CGFloat)maxForce {
    [self handleFT:force maxForce:maxForce];
}
- (void) forceTouchRecognizer:(DFContinuousForceTouchGestureRecognizer*)recognizer didCancelWithForce:(CGFloat)force maxForce:(CGFloat)maxForce {
    //[pd sendFloat:0 toReceiver:@"stiffness"];
}
- (void) forceTouchRecognizer:(DFContinuousForceTouchGestureRecognizer*)recognizer didEndWithForce:(CGFloat)force maxForce:(CGFloat)maxForce {
    //[pd sendFloat:0 toReceiver:@"stiffness"];
}
- (void) forceTouchDidTimeout:(DFContinuousForceTouchGestureRecognizer*)recognizer {
}

- (void)handleFT:(CGFloat)force maxForce:(CGFloat)maxForce {
    CGFloat f = force/maxForce;
    CGFloat min = 0;
    CGFloat max = 2000.;
    f = f * (max-min);
    f = f + min;
    //NSLog(@"%f",f);
    [pd sendFloat:f toReceiver:@"stiffness"];
}
@end
