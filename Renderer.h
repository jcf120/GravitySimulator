//
//  Renderer.h
//
//  Created by Jocelyn Clifford-Frith on 04/11/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Gravity.h"
#import <OpenGL/gl.h>

struct vert {
	GLdouble v[3];
	GLdouble n[3];
};
typedef struct vert Vert;

struct mesh {
	Vert *verts;
	GLbyte *index;
	GLsizei count;
	GLsizei elements;
};
typedef struct mesh Mesh;

Mesh sphereMesh(double radius, unsigned segments);

@interface Renderer : NSOpenGLView {
	
	Mesh sphere;
	Animation animation;
	NSMutableArray *bodiesData;
	BOOL animating;
	
	// Camera controls
	GLdouble camZoom;
	GLdouble camAngX;
	GLdouble camAngY;
}

- (void) updateProjection;

@property (readwrite) Animation animation;
@property (readwrite) NSInteger currentFrame;
@property (retain) NSMutableArray *bodiesData;
@property (readwrite) BOOL animating;

@end
