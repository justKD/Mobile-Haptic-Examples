//
//  NNStringView.m
//  NNStringView
//
//  Copyright © 2015 notnatural, LLC. All rights reserved.
//

#import "NNStringView.h"

@implementation NNStringView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self setDefaults];
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    stringLayers = [[NSMutableArray alloc] initWithCapacity:self.numberOfStrings];
    [self drawStringView];
}

- (void)setDefaults
{
    self.backgroundColor = [UIColor whiteColor];
    self.isVertical = NO;
    self.hasEndPointPadding = YES;
    self.stringWidth = 2;
    self.stringColor = [UIColor blueColor];
    self.numberOfStrings = 10;
}

- (void)drawStringView
{
    CGFloat originX = 0;
    CGFloat originY = 0;
    CGFloat endX = 0;
    CGFloat endY = 0;
    CGFloat pad = 0;
    
    if (self.hasEndPointPadding) {
        pad = 4;
    }
    
    if (!self.stringPositions) {
        for (int i = 1; i < (self.numberOfStrings + 1); i++) {
            if (!self.isVertical) {
                originX = self.bounds.origin.x + pad;
                originY = (self.bounds.size.height / (self.numberOfStrings+1)) * i;
                endX = self.bounds.size.width - pad;
                endY = originY;
            } else {
                originX = (self.bounds.size.width / (self.numberOfStrings+1)) * i;
                originY = self.bounds.origin.y + pad;
                endX = originX;
                endY = self.bounds.size.height - pad;
            }
            
            CGPoint origin = CGPointMake(originX, originY);
            CGPoint end = CGPointMake(endX, endY);
            
            [self drawLineAtOrigin:origin end:end];
        }
    } else {
        if (self.stringPositions.count == self.numberOfStrings) {
            for (int i = 0; i < [self.stringPositions count]; i++) {
                CGFloat pos = [[self.stringPositions objectAtIndex:i] floatValue];
                NSLog(@"%f",pos);
                if (!self.isVertical) {
                    originX = self.bounds.origin.x + pad;
                    originY = self.bounds.size.height * pos;
                    endX = self.bounds.size.width - pad;
                    endY = originY;
                } else {
                    originX = self.bounds.size.width * pos;;
                    originY = self.bounds.origin.y + pad;
                    endX = originX;
                    endY = self.bounds.size.height - pad;
                }
                
                CGPoint origin = CGPointMake(originX, originY);
                CGPoint end = CGPointMake(endX, endY);
                
                [self drawLineAtOrigin:origin end:end];
            }
        }
    }

}

- (void)drawLineAtOrigin:(CGPoint)origin end:(CGPoint)end
{
    UIGraphicsBeginImageContext(self.bounds.size);
    
    CAShapeLayer *layer = [CAShapeLayer layer];
    
    layer.path = [[self makeLinePathWithOrigin:origin end:end] CGPath];
    layer.strokeColor = [self.stringColor CGColor];
    layer.lineWidth = self.stringWidth;
    layer.lineJoin = kCALineJoinBevel;
    layer.drawsAsynchronously = YES;
    layer.lineCap = kCALineCapRound;

    [stringLayers addObject:layer];
    [self.layer addSublayer:[stringLayers lastObject]];
    
    UIGraphicsEndImageContext();
}

- (UIBezierPath *)makeLinePathWithOrigin:(CGPoint)origin end:(CGPoint)end {
    UIBezierPath* path = [UIBezierPath bezierPath];
    
    [path moveToPoint: CGPointMake(origin.x, origin.y)];
    [path addCurveToPoint: CGPointMake(end.x, end.y) controlPoint1: CGPointMake(origin.x, origin.y) controlPoint2: CGPointMake(end.x, end.y)];
    [path stroke];
    
    return path;
}

@end
