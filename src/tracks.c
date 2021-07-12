#include <stdio.h>
#include <string.h>
struct Point { float x; float y; };
struct Bezier { struct Point p1; struct Point p2; struct Point p3; };
struct Bezier curves[40];
char section[10][20], param[10][20], value[10][20];
 
//To calculate point [t] of the Bezier curve:
// A*(1-t)^2 + B*2t(1-t) + C*t^2
 
int get_track(char *track, struct Bezier track_shape[]){
 
	FILE *fp;

	long unsigned i = 0, x;
	int len = sizeof(struct Bezier);
	int index[40], width[40], test1, test2, test3, test4, test5, test_full;
	for(x = 0; x < 10; x++) {
	strcpy(section[x], "                      ");
	strcpy(param[x], "                        ");
	strcpy(value[x], "                        ");
 	}
	fp = fopen(track, "r");
 
	if (fp == NULL) {
		perror("No such file or directory");
	}
	
	test1 = fscanf(fp, "[%[^]]]", section[0]);
	test2 = fscanf(fp, " %5s - %5s", param[0], value[0]);

	test3 = fscanf(fp, " [ %8s ] ", section[1]);
	
	test4 = fscanf(fp, " (%1i) (%f, %f) (%f, %f) (%f, %f) (%1i) ", &index[i], &curves[0].p1.x, &curves[0].p1.y, &curves[0].p2.x, 
	&curves[0].p2.y, &curves[0].p3.x, &curves[0].p3.y, &width[i]);
 
	test5 = fscanf(fp, " (%1i) (%f, %f) (%f, %f) (%f, %f) (%1i) + %s = %s ", &index[i], &curves[0].p1.x, &curves[0].p1.y, &curves[0].p2.x, 
	&curves[0].p2.y, &curves[0].p3.x, &curves[0].p3.y, &width[i], param[1],value[1]);
	for (i = 0; i < (sizeof(curves) / len); i++) {
		track_shape[i] = curves[i];
	};
	test_full = test1 + test2 + test3 + test4 + test5;
	return(test_full);
}
 
