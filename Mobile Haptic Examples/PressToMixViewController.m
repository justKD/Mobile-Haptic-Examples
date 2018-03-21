//
//  PressToMixViewController.m
//  Mobile Haptic Examples
//
//  Copyright © 2015 notnatural, LLC. All rights reserved.
//

#import "PressToMixViewController.h"
#import "nnKit.h"
#import "NNEasyPD.h"
#import "DFContinuousForceTouchGestureRecognizer.h"
#import "FaustLoader.h"

@interface PressToMixViewController () <DFContinuousForceTouchDelegate>
{
    NNEasyPD *pd;
    
    UIView *q1;
    UIView *q2;
    UIView *q3;
    UIView *q4;
    
    UIView *gridView;
}

@end

@implementation PressToMixViewController

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}
-(BOOL)shouldAutorotate {
    return NO;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupPD];
    [self createUI];
}

- (void)setupPD {
    pd = [[NNEasyPD alloc] init];
    [FaustLoader setupGts];
    NSString *patchString = @"demo-using-hsliders~.pd";
    [pd loadPatch:patchString];

    NSArray *files = @[[[NSBundle mainBundle] pathForResource:@"zhappy-drum-loop" ofType:@"wav"],
                       [[NSBundle mainBundle] pathForResource:@"zdubstep-drum-loop" ofType:@"wav"],
                       [[NSBundle mainBundle] pathForResource:@"zelectro-house-loop" ofType:@"wav"],
                       [[NSBundle mainBundle] pathForResource:@"zsax-chinese" ofType:@"wav"]
                       ];

    [pd sendMessage:[files objectAtIndex:0] withArguments:nil toReceiver:[NSString stringWithFormat:@"1.url"]];
    [pd sendMessage:[files objectAtIndex:1] withArguments:nil toReceiver:[NSString stringWithFormat:@"2.url"]];
    [pd sendMessage:[files objectAtIndex:2] withArguments:nil toReceiver:[NSString stringWithFormat:@"3.url"]];
    [pd sendMessage:[files objectAtIndex:3] withArguments:nil toReceiver:[NSString stringWithFormat:@"4.url"]];
}

- (void)createUI {
    [pd sendFloat:self.version toReceiver:@"version"];
    
    CGFloat pad = 4;
    CGFloat width = (SW()/2)-pad-(pad/2);
    CGFloat height = (SH()/2)-pad-(pad/2);
    CGFloat centerHSplit = (SH()/2)+pad;
    CGFloat centerVSplit = (SW()/2)+pad;
    
    gridView = [[UIView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:gridView];
    
    q1 = [[UIView alloc] initWithFrame:CGRectMake(pad, pad, width, height)];
    q2 = [[UIView alloc] initWithFrame:CGRectMake(centerVSplit, pad, width, height)];
    q3 = [[UIView alloc] initWithFrame:CGRectMake(pad, centerHSplit, width, height)];
    q4 = [[UIView alloc] initWithFrame:CGRectMake(centerVSplit, centerHSplit, width, height)];
    
    CGFloat cornerRadius = 6;
    q1.layer.cornerRadius = cornerRadius;
    q2.layer.cornerRadius = cornerRadius;
    q3.layer.cornerRadius = cornerRadius;
    q4.layer.cornerRadius = cornerRadius;
    
    q1.backgroundColor = UIColorFromHex(0xD33257);
    q2.backgroundColor = UIColorFromHex(0x3D8EB9);
    q3.backgroundColor = UIColorFromHex(0x71BA51);
    q4.backgroundColor = UIColorFromHex(0xFEC606);
    
    q1.tag = 1;
    q2.tag = 2;
    q3.tag = 3;
    q4.tag = 4;
    
    if (self.version == 1) {
        DFContinuousForceTouchGestureRecognizer* ft1 = [[DFContinuousForceTouchGestureRecognizer alloc] init];
        ft1.forceTouchDelegate = self;
        DFContinuousForceTouchGestureRecognizer* ft2 = [[DFContinuousForceTouchGestureRecognizer alloc] init];
        ft2.forceTouchDelegate = self;
        DFContinuousForceTouchGestureRecognizer* ft3 = [[DFContinuousForceTouchGestureRecognizer alloc] init];
        ft3.forceTouchDelegate = self;
        DFContinuousForceTouchGestureRecognizer* ft4 = [[DFContinuousForceTouchGestureRecognizer alloc] init];
        ft4.forceTouchDelegate = self;
        
        [q1 addGestureRecognizer:ft1];
        [q2 addGestureRecognizer:ft2];
        [q3 addGestureRecognizer:ft3];
        [q4 addGestureRecognizer:ft4];
        
        [pd sendFloat:1 toReceiver:@"volume"];
    } else {
        if (self.version == 3) {
            DFContinuousForceTouchGestureRecognizer* ft1 = [[DFContinuousForceTouchGestureRecognizer alloc] init];
            ft1.forceTouchDelegate = self;
            [gridView addGestureRecognizer:ft1];
            [pd sendFloat:1 toReceiver:@"pulseon"];
        } else {
            [pd sendFloat:1 toReceiver:@"volume"];
        }
    }
    
    [gridView addSubview:q1];
    [gridView addSubview:q2];
    [gridView addSubview:q3];
    [gridView addSubview:q4];
    
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    [button addTarget:self
               action:@selector(goBack:)
     forControlEvents:UIControlEventTouchUpInside];
    [button setTitle:@"Back" forState:UIControlStateNormal];
    button.frame = CGRectMake(10, 20, 60, 40);
//    button.backgroundColor = [UIColor redColor];
//    button.layer.cornerRadius = 8;
    [self.view addSubview:button];
}

- (void)handleFade:(CGPoint)loc {
    
    /*

     --------------------
     |p3              p2|
     |                  |
     |                  |
     |                  |
     |      pL          |
     |        \         |
     |         \        |
     |          \       |
     |           d1     |
     |             \    |
     |              \   |
     |p4              p1|
     --------------------

     pL = touch point
     d1 = distance from touch to p1
     x1 = x of d1
     y1 = y of d1
     g1 = gain of speaker at p1
     gv = volume scale

     dz = 0.01
           ___________________________
     gs = / g1^2 + g2^2 + g3^2 + g4^2
           ___________________________
     d1 = / (x1 - xL)^2 + (y1 - xL)^2
     g1 = 1 / MAX(dz, d1)
     g1 = (g1/gs) * gv
     
    */
    
    CGRect frame = self.view.frame;
    
    CGPoint pL = loc;
    pL = CGPointMake(pL.x/frame.size.width, pL.y/frame.size.height);
    CGFloat dz = 0.01;
    
    CGPoint p1 = CGPointMake(frame.size.width/frame.size.width, frame.size.height/frame.size.height);
    CGPoint p2 = CGPointMake(frame.size.width/frame.size.width, 0);
    CGPoint p3 = CGPointMake(0, 0);
    CGPoint p4 = CGPointMake(0, frame.size.height/frame.size.height);
    
    CGFloat x1 = (p1.x - pL.x) * (p1.x - pL.x);
    CGFloat y1 = (p1.y - pL.y) * (p1.y - pL.y);
    CGFloat d1 = sqrtf(x1+y1);
    CGFloat g1 = 1 / MAX(dz, d1);
    
    CGFloat x2 = (p2.x - pL.x) * (p2.x - pL.x);
    CGFloat y2 = (p2.y - pL.y) * (p2.y - pL.y);
    CGFloat d2 = sqrtf(x2+y2);
    CGFloat g2 = 1 / MAX(dz, d2);
    
    CGFloat x3 = (p3.x - pL.x) * (p3.x - pL.x);
    CGFloat y3 = (p3.y - pL.y) * (p3.y - pL.y);
    CGFloat d3 = sqrtf(x3+y3);
    CGFloat g3 = 1 / MAX(dz, d3);
    
    CGFloat x4 = (p4.x - pL.x) * (p4.x - pL.x);
    CGFloat y4 = (p4.y - pL.y) * (p4.y - pL.y);
    CGFloat d4 = sqrtf(x4+y4);
    CGFloat g4 = 1 / MAX(dz, d4);
    
    CGFloat gs = sqrtl((g1*g1)+(g2*g2)+(g3*g3)+(g4*g4));
    
    CGFloat gv = 1;
    g1 = (g1/gs) * gv;
    g2 = (g2/gs) * gv;
    g3 = (g3/gs) * gv;
    g4 = (g4/gs) * gv;
    
    //NSLog(@"\n%.2f %.2f %.2f %.2f",g1,g2,g3,g4);
    
    g1 = [self scaleGain:g1];
    g2 = [self scaleGain:g2];
    g3 = [self scaleGain:g3];
    g4 = [self scaleGain:g4];
    
    [pd sendFloat:g1 toReceiver:@"ft1"];
    [pd sendFloat:g2 toReceiver:@"ft2"];
    [pd sendFloat:g3 toReceiver:@"ft3"];
    [pd sendFloat:g4 toReceiver:@"ft4"];
}

- (CGFloat)scaleGain:(CGFloat)input {
    CGFloat output = 1 - input;
    output = input * .01;
    output = output - .005;
    output = 0 - output;
    
    return output;
}

- (void)goBack:(UIButton*)sender {
    [pd resetPD];
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    if (self.version != 1) {
        UITouch *touch = [[event allTouches] anyObject]; //[[event allTouches] anyObject];
        
        [pd sendFloat:1 toReceiver:@"onoff"];
        [self handleFade:[touch locationInView:gridView]];
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    if (self.version != 1) {
        UITouch *touch = [[event allTouches] anyObject]; //[[touches allObjects] objectAtIndex:0];
        [self handleFade:[touch locationInView:gridView]];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    if (self.version != 1) {
        [pd sendFloat:0 toReceiver:@"onoff"];
        
        CGFloat off = .005;
        [pd sendFloat:off toReceiver:@"ft1"];
        [pd sendFloat:off toReceiver:@"ft2"];
        [pd sendFloat:off toReceiver:@"ft3"];
        [pd sendFloat:off toReceiver:@"ft4"];
    }
}

- (void) forceTouchRecognized:(DFContinuousForceTouchGestureRecognizer*)recognizer {
    // Required delegate protocol
    [pd sendFloat:1 toReceiver:@"onoff"];
}
- (void) forceTouchRecognizer:(DFContinuousForceTouchGestureRecognizer*)recognizer didStartWithForce:(CGFloat)force maxForce:(CGFloat)maxForce {
    
    [self handleForceTouch:force maxForce:maxForce recognizer:recognizer];
}
- (void) forceTouchRecognizer:(DFContinuousForceTouchGestureRecognizer*)recognizer didMoveWithForce:(CGFloat)force maxForce:(CGFloat)maxForce {
    [self handleForceTouch:force maxForce:maxForce recognizer:recognizer];
}
- (void) forceTouchRecognizer:(DFContinuousForceTouchGestureRecognizer*)recognizer didCancelWithForce:(CGFloat)force maxForce:(CGFloat)maxForce {
    
    [self handleForceReset:recognizer];
}
- (void) forceTouchRecognizer:(DFContinuousForceTouchGestureRecognizer*)recognizer didEndWithForce:(CGFloat)force maxForce:(CGFloat)maxForce {
    
    [self handleForceReset:recognizer];
}
- (void) forceTouchDidTimeout:(DFContinuousForceTouchGestureRecognizer*)recognizer {
    
    [self handleForceReset:recognizer];
}

- (void)handleForceTouch:(CGFloat)force maxForce:(CGFloat)maxForce recognizer:(DFContinuousForceTouchGestureRecognizer*)sender
{
    if (self.version == 1) {
        CGFloat ft = force/maxForce;
        ft = sqrt(ft);
        
        ft = 1 - ft;
        ft = ft - .95;
        ft = ft * .01;
        NSLog(@"%f",ft);
        
        switch (sender.view.tag) {
            case 1:
                [pd sendFloat:ft toReceiver:@"ft1"];
                break;
            case 2:
                [pd sendFloat:ft toReceiver:@"ft2"];
                break;
            case 3:
                [pd sendFloat:ft toReceiver:@"ft3"];
                break;
            case 4:
                [pd sendFloat:ft toReceiver:@"ft4"];
                break;
                
            default:
                break;
        }
        //NSLog(@"%f %ld",ft,sender.view.tag);
    } else {
        CGFloat f = force/maxForce;
        f = 1 - f;
        CGFloat min = 50;
        CGFloat max = 500;
        f = f * (max-min);
        f = f + min;
        [pd sendFloat:f toReceiver:@"pulse"];
        
        f = force/maxForce;
        min = .1;
        max = 10.;
        f = f * (max-min);
        f = f + min;
        [pd sendFloat:f toReceiver:@"volume"];
    }
}

- (void)handleForceReset:(DFContinuousForceTouchGestureRecognizer*)sender
{
    //[pd sendFloat:0 toReceiver:@"onoff"];
    
    CGFloat off = .05;
    switch (sender.view.tag) {
        case 1:
            [pd sendFloat:off toReceiver:@"ft1"];
            break;
        case 2:
            [pd sendFloat:off toReceiver:@"ft2"];
            break;
        case 3:
            [pd sendFloat:off toReceiver:@"ft3"];
            break;
        case 4:
            [pd sendFloat:off toReceiver:@"ft4"];
            break;
            
        default:
            break;
    }
}

@end
