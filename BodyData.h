//
//  BodyData.h
//  ThreeBodyGravity
//
//  Created by Jocelyn Clifford-Frith on 08/11/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Gravity.h"


@interface BodyData : NSObject {
	
	NSString *name;
	double mass;
	Vector3d position;
	Vector3d velocity;
	
	// Colour
	double red;
	double green;
	double blue;
}

@property (retain) NSString *name;
@property (readwrite) double mass;
@property (readwrite) Vector3d position;
@property (readwrite) Vector3d velocity;

@property (readonly) double red;
@property (readonly) double green;
@property (readonly) double blue;

@end
