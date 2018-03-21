//
//  ViewController.m
//  Mobile Haptic Examples
//
//  Copyright © 2015 notnatural, LLC. All rights reserved.
//

#import "ViewController.h"
#import "nnKit.h"
#import "PluckViewController.h"
#import "PressToMixViewController.h"

@interface ViewController () {
    UIButton *button1;
    UIButton *button2;
    UIButton *button3;
    UIButton *backButton;
    
    CGFloat numButtons;
}

@end

@implementation ViewController

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}
-(BOOL)shouldAutorotate {
    return NO;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    numButtons = 3;
    numButtons = numButtons + 1;
    [self makeButtons];
}

- (void)makeButtons {

    button1 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 1) fontSize:24 title:@"Pluck Harp 10" method:@"handlePluckVC:" fromClass:self];
    button1.tag = 0;
    
    button2 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 2) fontSize:24 title:@"Pluck One String" method:@"handlePluckVC:" fromClass:self];
    button2.tag = 1;
    
    button3 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 3) fontSize:24 title:@"Press to Mix" method:@"handlePressToMixVC:" fromClass:self];

    [self addButtons];
}

- (void)handlePluckVC:(UIButton*)sender {
    [nnKit animateViewBigJiggle:sender];
    PluckViewController *vc = [[PluckViewController alloc] init];
    vc.patchValue = (int)sender.tag;
    vc.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;
    [self presentViewController:vc animated:YES completion:nil];
}

//- (void)handlePressToMix:(UIButton*)sender {
//    
//    [self removeButtons];
//    
//    backButton = [nnKit makeButtonWithFrame:CGRectMake(15, 20, 50, 25) fontSize:24 title:@"Back" method:@"goBack:" fromClass:self];
//    [nnKit animateViewGrowAndShow:backButton or:nil completion:nil];
//    [self.view addSubview:backButton];
//    
//    button1 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 1) fontSize:24 title:@"4 Region Force Touch" method:@"handlePressToMixVC:" fromClass:self];
//    button1.tag = 1;
//    
//    button2 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 2) fontSize:24 title:@"2D Crossfade" method:@"handlePressToMixVC:" fromClass:self];
//    button2.tag = 2;
//    
//    button3 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 3) fontSize:24 title:@"2D Crossfade with Force" method:@"handlePressToMixVC:" fromClass:self];
//    button3.tag = 3;
//    
//    [self addButtons];
//}

- (void)handlePressToMixVC:(UIButton*)sender {
    [nnKit animateViewBigJiggle:sender];
    PressToMixViewController *vc = [[PressToMixViewController alloc] init];
    vc.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;
    //vc.version = (int)sender.tag;
    vc.version = 1;
    [self presentViewController:vc animated:YES completion:nil];
}

- (void)goBack:(UIButton*)sender {
    
    [self removeButtons];

    button1 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 1) fontSize:24 title:@"Pluck Harp 10" method:@"handlePluckVC:" fromClass:self];
    button1.tag = 0;
    
    button2 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 2) fontSize:24 title:@"Pluck One String" method:@"handlePluckVC:" fromClass:self];
    button2.tag = 1;
    
    button3 = [nnKit makeButtonWithCenter:CGPointMake(SW()/2, (SH()/numButtons) * 3) fontSize:24 title:@"Press to Mix" method:@"handlePressToMix:" fromClass:self];
    
    [self addButtons];
}

- (void)removeButtons {
    if (backButton) {
        [nnKit animateViewShrinkAndWink:backButton or:nil andRemoveFromSuperview:YES completion:nil];
        backButton = nil;
    }
    
    [nnKit animateViewShrinkAndWink:button1 or:nil andRemoveFromSuperview:YES completion:nil];
    [nnKit animateViewShrinkAndWink:button2 or:nil andRemoveFromSuperview:YES completion:nil];
    [nnKit animateViewShrinkAndWink:button3 or:nil andRemoveFromSuperview:YES completion:nil];
    
    button1 = nil;
    button2 = nil;
    button3 = nil;
}

- (void)addButtons {
    [nnKit animateViewGrowAndShow:button1 or:nil completion:nil];
    [nnKit animateViewGrowAndShow:button2 or:nil completion:nil];
    [nnKit animateViewGrowAndShow:button3 or:nil completion:nil];
    
    [self.view addSubview:button1];
    [self.view addSubview:button2];
    [self.view addSubview:button3];
}

@end
