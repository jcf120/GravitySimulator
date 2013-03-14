//
//  BodyData.m
//  ThreeBodyGravity
//
//  Created by Jocelyn Clifford-Frith on 08/11/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "BodyData.h"


@implementation BodyData

@synthesize name;
@synthesize mass;
@synthesize position;
@synthesize velocity;

@synthesize red;
@synthesize green;
@synthesize blue;

- (id) init
{
	if (self = [super init]) {
		name = @"Untitled";
		mass = 1.0;
		
		red		= 1.0/255.0*(rand()%255);
		green	= 1.0/255.0*(rand()%255);
		blue	= 1.0/255.0*(rand()%255);
	}
	return self;
}

@end
