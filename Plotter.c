/*
 *  Plotter.c
 *  ThreeBodyGravity
 *
 *  Created by Jocelyn Clifford-Frith on 29/11/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Plotter.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void plot(double **points, int point_count, int sets, double dt)
{
	const char *data_dir = "/tmp/GravData.dat";
	FILE *data = fopen(data_dir, "w");
	if (!data)
		return;
	
	double xaxis = 0.0;	
	for (int n=0; n<point_count; n++) {
		fprintf(data, "%.15e ",xaxis);
		for (int m=0; m<sets; m++) {
			double *plot = points[m];
			fprintf(data, "%.15e ",plot[n]);
		}
		fprintf(data, "\n");
		xaxis += dt;
	}
	
	fclose(data);
	
	const char *script_dir = "/tmp/GravSim.gp";
	FILE *script = fopen(script_dir, "w");
	if (!script)
		return;
	
	fprintf(script, "set xlabel \"Time (yr)\"\n"
			"set ylabel \"Obital Energy\"\n"
			"set title \"Obital Energy Against Time\"\n"
			"plot \"/tmp/GravData.dat\" using 1:2 with lines");
	for (int n=1; n<sets; n++)
		fprintf(script, ", \"/tmp/GravData.dat\" using 1:%i with lines",n+2);
	fprintf(script, "\n");
	
	fclose(script);
	system("/usr/local/bin/gnuplot /tmp/GravSim.gp -persist");
}