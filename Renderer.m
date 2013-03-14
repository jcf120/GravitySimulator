//
//  Renderer.m
//
//  Created by Jocelyn Clifford-Frith on 04/11/2010.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "Renderer.h"

#import "BodyData.h"
#define DEGREES_TO_RADIANS(__ANGLE) ((__ANGLE) / 180.0 * M_PI)

@implementation Renderer

- (void) awakeFromNib
{	
	[[self openGLContext] makeCurrentContext];
	[self updateProjection];
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	sphere = sphereMesh(0.1, 10);
	
	// camera
	camZoom	= 10.0;
	camAngX = 20.0;
	camAngY = 40.0;
	
	// lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    const GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    const GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    const GLfloat specular[] = {0.7f, 0.7f, 0.7f, 1.0f};
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	
	// material
    glEnable(GL_COLOR_MATERIAL);
    const GLfloat material[] = {0.0f, 1.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,material);
}

- (void) dealloc
{
	free(sphere.verts);
	free(sphere.index);
	[super dealloc];
}

@synthesize animation;
@synthesize currentFrame;
@synthesize bodiesData;
@synthesize animating;

- (void) updateProjection
{
	CGRect rect = self.bounds;
	const GLdouble zNear = 0.1, zFar = 2000.0, fieldOfView = 60.0;
	GLdouble size;
	size = zNear * tanf(DEGREES_TO_RADIANS(fieldOfView) / 2.0);
	glViewport(0, 0, rect.size.width, rect.size.height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glFrustum(-size,
			  size,
			  -size / (rect.size.width / rect.size.height),
			  size / (rect.size.width / rect.size.height),
			  zNear,
			  zFar);
}

- (void) update
{
	[self updateProjection];
	[super update];
}

- (void) drawAxis
{
	const GLdouble axis_length = 2.0;
	const GLdouble axis[] = {
		-axis_length,0.0,0.0,axis_length,0.0,0.0,
		0.0,-axis_length,0.0,0.0,axis_length,0.0,
		0.0,0.0,-axis_length,0.0,0.0,axis_length
	};
	glColor4d(1.0, 1.0, 0.0, 1.0);
	glVertexPointer(3, GL_DOUBLE, 0, axis);
	glDrawArrays(GL_LINES, 0, 6);
}

- (void) drawRect: (NSRect) dirtyRect
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw Code
	
	glPushMatrix();
	glTranslated(0.0, 0.0, -camZoom);
	glRotated(camAngX, 1.0, 0.0, 0.0);
	glRotated(camAngY, 0.0, 1.0, 0.0);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	[self drawAxis];
	glEnableClientState(GL_NORMAL_ARRAY);
	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertexPointer(3, GL_DOUBLE, sizeof(Vert), sphere.verts[0].v);
	glNormalPointer(GL_DOUBLE, sizeof(Vert), sphere.verts[0].n);
	
	// If animating
	if (animating) {
		for (int n=0; n<animation.body_count; n++) {
			Vector3d *b = animation.bodies[n];
			Vector3d p = b[currentFrame];
			
			BodyData *bd = [bodiesData objectAtIndex:n];
			glColor3f(bd.red, bd.green, bd.blue);
			
			glPushMatrix();
			glTranslated(p.x, p.y, p.z);
			glDrawElements(GL_TRIANGLES, sphere.elements, GL_UNSIGNED_BYTE, sphere.index);
			glPopMatrix();
		}
		
		// Draw Paths
		glDisableClientState(GL_NORMAL_ARRAY);
		for (int n=0; n<animation.body_count; n++) {
			
			BodyData *bd = [bodiesData objectAtIndex:n];
			glColor3f(bd.red, bd.green, bd.blue);
			
			glVertexPointer(3, GL_DOUBLE, sizeof(Vector3d), animation.bodies[n]);
			glDrawArrays(GL_LINE_STRIP, 0, currentFrame);
		}
		glEnableClientState(GL_NORMAL_ARRAY);
	}
	// Else Static Frame
	else {
		for (BodyData *b in bodiesData) {
			
			glColor3f(b.red, b.green, b.blue);
			
			glPushMatrix();
			glTranslated([b position].x, [b position].y, [b position].z);
			glDrawElements(GL_TRIANGLES, sphere.elements, GL_UNSIGNED_BYTE, sphere.index);
			glPopMatrix();
		}
	}

	
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glPopMatrix();
	
	glFlush();
}

// Mouse controls

- (void)scrollWheel:(NSEvent *)theEvent
{
	double wheelDelta = [theEvent deltaX] +[theEvent deltaY] + [theEvent deltaZ];
	camZoom -= wheelDelta/4.0;
	[self setNeedsDisplay:YES];
}

- (void) mouseDragged:(NSEvent *)theEvent
{
	//double mouseDelta = [theEvent deltaX] +[theEvent deltaY] + [theEvent deltaZ];
	camAngX += [theEvent deltaY];
	camAngY += [theEvent deltaX];
	[self setNeedsDisplay:YES];
}

@end


Mesh sphereMesh(double radius, unsigned seg)
{
	Mesh sphere;
	
	//calculate verts
	sphere.count = 2*seg*(seg-1)+2;
	sphere.verts = malloc(sizeof(Vert) * sphere.count);
	
	sphere.verts[0].v[1] = radius;
	for (int n=1, i=1; n<seg; n++) {
		for (int m=0; m<seg*2; m++) {
			GLdouble *v = sphere.verts[i].v;
            v[0] = radius * sin(-M_PI*m/seg) * sin(-M_PI*n/seg);
            v[2] = radius * cos(-M_PI*m/seg) * sin(-M_PI*n/seg);
            v[1] = radius * cos(M_PI*n/seg);
			i++;
		}
	}
	sphere.verts[2*seg*(seg-1)+1].v[1] = -radius;
	
	// calculate normals
	for (int n=0; n<sphere.count; n++) {
		Vert *v = &(sphere.verts[n]);
		GLdouble x, y, z;
		x=v->v[0]; y=v->v[1]; z=v->v[2];
		GLdouble mag = sqrt(x*x+y*y+z*z);
		v->n[0]=x/mag; v->n[1]=y/mag; v->n[2]=z/mag;
	}
	
	
	// calculate index
	sphere.elements = 12*seg*(seg-1);
	sphere.index = malloc(sizeof(GLuint) * sphere.elements);
	
	// top triangles
	for (int n=1, i=0; n<seg*2+1; n++) {
		sphere.index[i++]=0;
		sphere.index[i++]=n+1;
		sphere.index[i++]=n;
	}
	sphere.index[seg*6-2]=1;
	
	// middle triangles
	for (int n=0, i=6*seg; n<seg-2; n++) {
		for (int m=1; m<seg*2+1; m++) {
			sphere.index[i++]=n*seg*2+m;
			sphere.index[i++]=(n+1)*seg*2+m+1;
			sphere.index[i++]=(n+1)*seg*2+m;
			sphere.index[i++]=n*seg*2+m+1;
			sphere.index[i++]=(n+1)*seg*2+m+1;
			sphere.index[i++]=n*seg*2+m;
		}
		sphere.index[(12*n+18)*seg-5] = (n+1)*seg*2+1;
		sphere.index[(12*n+18)*seg-3] = n*seg*2+1;
		sphere.index[(12*n+18)*seg-2] = (n+1)*seg*2+1;
	}
	
	// bottom triangles
	for (int n=1, i=6*seg*(2*seg-3); n<seg*2+1; n++) {
		sphere.index[i++]=2*seg*(seg-2)+n;
		sphere.index[i++]=2*seg*(seg-2)+n+1;
		sphere.index[i++]=2*seg*(seg-1)+1;
	}
	sphere.index[sphere.elements-2] = 2*seg*(seg-2)+1;
	
	return sphere;
}