//
//  Controller.m
//  ThreeBodyGravity
//
//  Created by Jocelyn Clifford-Frith on 08/11/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "Controller.h"


@implementation Controller

- (id) init
{
    self = [super init];
	if (self) {
		frameNumber = 0;
		bodiesData = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void) awakeFromNib
{
	renderer.animating = NO;
	renderer.bodiesData = bodiesData;
}

- (IBAction) run: (id) sender
{
	// Prevent interuption
	[runButton setEnabled:NO];
	[table setEnabled:NO];
	
	// Format data for simulator
	unsigned body_count = [bodiesData count];
	Body *bodies = calloc(sizeof(Body), body_count);
	for (int n=0; n<body_count; n++) {
		bodies[n].mass = [(BodyData*)[bodiesData objectAtIndex:n] mass];
		bodies[n].p = [(BodyData*)[bodiesData objectAtIndex:n] position];
		bodies[n].newp = bodies[n].p;
		bodies[n].v = [(BodyData*)[bodiesData objectAtIndex:n] velocity];
	}
	double duration = [durationField doubleValue];
	double dt = [dtField doubleValue];
	
	// Time processing
	NSTimeInterval startTime = [NSDate timeIntervalSinceReferenceDate];
	
	animation = simulate(bodies, body_count, dt, duration);
	
	// Update timer display
	NSTimeInterval processingTime = [NSDate timeIntervalSinceReferenceDate] - startTime;
	[timeLabel setFloatValue:processingTime];

	// Plot Energy graph
	//double *points[3] = {animation.orbital_energy,animation.kinetic_energy,animation.potential_energy};
	//plot(points, animation.frame_count, 1, [dtField doubleValue]);
	
	
	renderer.animation = animation;
	renderer.animating = YES;
    FPL = 1.0E-2 / dt;
    if (FPL < 1) FPL = 1;
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
													  target:self
													selector:@selector(animate)
													userInfo:NULL
													 repeats:YES];
	[runButton setTitle:@"Stop"];
	[runButton setAction:@selector(stop)];
	[runButton setEnabled:YES];
}

- (void) reset
{	
	[runButton setEnabled:NO];
	
	// Stop Renderer drawing animation
	renderer.animating = NO;
	
	// Responsible for animation's memory
	free_animation(&animation);
	
	// Beginning of animation
	frameNumber = 0;
	
	// Return to run button
	[runButton setEnabled:NO];
	[runButton setAction:@selector(run:)];
	[runButton setTitle:@"Run"];
	[runButton setEnabled:YES];
	
	// Allow edits again
	[table setEnabled:YES];

	[renderer setNeedsDisplay:YES];
}

- (void) stop
{
	[runButton setEnabled:NO];
	
	// Stop timer
	[animationTimer invalidate];
	
	[runButton setTitle:@"Reset"];
	[runButton setAction:@selector(reset)];
	[runButton setEnabled:YES];
}
	

- (void) animate
{
	if (frameNumber>=animation.frame_count) {
		[self stop];
		return;
	}
	[lMomXLabel setFloatValue:animation.linear_momentum[frameNumber].x];
	[lMomYLabel setFloatValue:animation.linear_momentum[frameNumber].y];
	[lMomZLabel setFloatValue:animation.linear_momentum[frameNumber].z];
	[aMomXLabel setFloatValue:animation.angular_momentum[frameNumber].x];
	[aMomYLabel setFloatValue:animation.angular_momentum[frameNumber].y];
	[aMomZLabel setFloatValue:animation.angular_momentum[frameNumber].z];
	[energyLabel setFloatValue:animation.orbital_energy[frameNumber]];
	
	renderer.currentFrame = frameNumber;
	[renderer setNeedsDisplay:YES];
	
	frameNumber+=FPL;
}

// User data

- (IBAction) addBodyData: (id) sender
{
	BodyData *b = [[BodyData alloc] init];
	[bodiesData addObject:b];
	[b release];
	[table reloadData];
	[renderer setNeedsDisplay:YES];
}

- (IBAction) removeBodyData: (id) sender
{
	// Check there is body to delete
	if ([bodiesData count]) {
		NSUInteger selectedRow = [table selectedRow];
		// Remove last object if none selected
		if (selectedRow == -1)
			[bodiesData removeLastObject];
		else
			[bodiesData removeObjectAtIndex:selectedRow];
		[table reloadData];
		[renderer setNeedsDisplay:YES];
	}
}

// Table methods

- (id)tableView:(NSTableView *)aTableView
objectValueForTableColumn:(NSTableColumn *)aTableColumn
			row:(NSInteger)rowIndex
{
	BodyData *b = [bodiesData objectAtIndex:rowIndex];
	if ([[aTableColumn identifier] isEqualToString:@"Body"]) {
		return b.name;
	}
	else if ([[aTableColumn identifier] isEqualToString:@"Mass"]) {
		return [NSNumber numberWithFloat:b.mass];
	}
	else if ([[aTableColumn identifier] isEqualToString:@"x"]) {
		return [NSNumber numberWithFloat:[b position].x];
	}
	else if ([[aTableColumn identifier] isEqualToString:@"y"]) {
		return [NSNumber numberWithFloat:[b position].y];
	}
	else if ([[aTableColumn identifier] isEqualToString:@"z"]) {
		return [NSNumber numberWithFloat:[b position].z];
	}
	else if ([[aTableColumn identifier] isEqualToString:@"vx"]) {
		return [NSNumber numberWithFloat:[b velocity].x];
	}
	else if ([[aTableColumn identifier] isEqualToString:@"vy"]) {
		return [NSNumber numberWithFloat:[b velocity].y];
	}
	else if ([[aTableColumn identifier] isEqualToString:@"vz"]) {
		return [NSNumber numberWithFloat:[b velocity].z];
	}
	return nil;
}

- (void)tableView:(NSTableView *)aTableView
   setObjectValue:anObject
   forTableColumn:(NSTableColumn *)aTableColumn
			  row:(NSInteger)rowIndex
{
	BodyData *b = [bodiesData objectAtIndex:rowIndex];
	if ([[aTableColumn identifier] isEqualToString:@"Body"]) {
		b.name = anObject;
	}
	else if ([[aTableColumn identifier] isEqualToString:@"Mass"]) {
		b.mass = [anObject doubleValue];
	}
	else if ([[aTableColumn identifier] isEqualToString:@"x"]) {
		Vector3d p = [b position];
		p.x = [anObject doubleValue];
		b.position = p;
	}
	else if ([[aTableColumn identifier] isEqualToString:@"y"]) {
		Vector3d p = [b position];
		p.y = [anObject doubleValue];
		b.position = p;
	}
	else if ([[aTableColumn identifier] isEqualToString:@"z"]) {
		Vector3d p = [b position];
		p.z = [anObject doubleValue];
		b.position = p;
	}
	else if ([[aTableColumn identifier] isEqualToString:@"vx"]) {
		Vector3d v = [b velocity];
		v.x = [anObject doubleValue];
		b.velocity = v;
	}
	else if ([[aTableColumn identifier] isEqualToString:@"vy"]) {
		Vector3d v = [b velocity];
		v.y = [anObject doubleValue];
		b.velocity = v;
	}
	else if ([[aTableColumn identifier] isEqualToString:@"vz"]) {
		Vector3d v = [b velocity];
		v.z = [anObject doubleValue];
		b.velocity = v;
	}
	
	// Update static render
	[renderer setNeedsDisplay:YES];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
    return [bodiesData count];
}

@end
