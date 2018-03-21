//
//  NNStringView.h
//  NNStringView
//
//  Copyright © 2015 notnatural, LLC. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NNStringView : UIView {
    NSMutableArray *stringLayers;
}

@property (nonatomic) BOOL isVertical;
@property (nonatomic) BOOL hasEndPointPadding;
@property (nonatomic) CGFloat stringWidth;
@property (nonatomic, strong) UIColor *stringColor;
@property (nonatomic) CGFloat numberOfStrings;
@property (nonatomic, strong) NSArray *stringPositions;

@end
