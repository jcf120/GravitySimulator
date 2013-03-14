//
//  Controller.h
//  ThreeBodyGravity
//
//  Created by Jocelyn Clifford-Frith on 08/11/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Gravity.h"
#import "Renderer.h"
#import "BodyData.h"
#import "Plotter.h"


@interface Controller : NSObject {
	
	Animation animation;
	IBOutlet Renderer *renderer;
	IBOutlet NSTextField *durationField;
	IBOutlet NSTextField *dtField;
	IBOutlet NSTextField *timeLabel;
	IBOutlet NSTextField *lMomXLabel;
	IBOutlet NSTextField *lMomYLabel;
	IBOutlet NSTextField *lMomZLabel;
	IBOutlet NSTextField *aMomXLabel;
	IBOutlet NSTextField *aMomYLabel;
	IBOutlet NSTextField *aMomZLabel;
	IBOutlet NSTextField *energyLabel;
	IBOutlet NSTableView *table;
	IBOutlet NSButton *runButton;
	
	IBOutlet NSButton *addButton;
	IBOutlet NSButton *removeButton;
	
	NSMutableArray *bodiesData;
	
	NSTimer *animationTimer;
	NSUInteger frameNumber;
    NSUInteger FPL;
}

- (IBAction) run: (id) sender;
- (void) animate;

- (IBAction) addBodyData: (id) sender;
- (IBAction) removeBodyData: (id) sender;

@end
