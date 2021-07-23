#include <stdio.h>
#include <string.h>
#include <stdbool.h>

enum SPEED { normal, boost, antiboost };

enum RACING_LINE {no, finish_line, checkpoint};

enum GUARDRAILS {empty, metal, electric};

enum BIOME {forest, desert, snow};

enum TIME {day, night};


struct Point {
  int x;
  int y;
};

struct Bezier {
  struct Point p1;
  struct Point p2;
  struct Point p3;
};

struct Segment {
	unsigned int index;
	
	struct Bezier shape;

	enum SPEED speed;
	enum RACING_LINE racing_line;
	enum GUARDRAILS guardrails;
};

struct Track {
	char name[20];

	enum BIOME biome;
	enum TIME time;
	short unsigned int max_players;
	
	struct Segment segments[40];
};

struct Track tracks[10];
struct Bezier curves[40];

char section[10][20], param[10][20], value[10][20];
char buffer[80][100];

// To calculate point [t] of the Bezier curve:
// A*(1-t)^2 + B*2t(1-t) + C*t^2

int get_track(char *track, struct Bezier track_shape[]) {

  FILE *fp;

  long unsigned i = 0, x, curvetrack = 0, sectiontrack = 0, valuetrack = 0;
  int len = sizeof(struct Bezier);
  int index[40], width[40], test_full = 0;
  for (x = 0; x < 10; x++) {
    strcpy(section[x], "");
    strcpy(param[x], "");
    strcpy(value[x], "");
  }
  for (x = 0; x < 80; x++) {
    strcpy(buffer[x], "");
  }

  fp = fopen(track, "r");

  if (fp == NULL) {
    perror("No such file or directory");
  }

  for (x = 0; x < 80 && (fgets(buffer[x], sizeof(buffer[x]), fp) != NULL);
       x++) {
    if (sscanf(buffer[x], " [%[^]]] ", section[sectiontrack]) != 1) {
      test_full += 1;
    } else {
      sectiontrack++;
    }

    if (sscanf(buffer[x], " %s - %s ", param[valuetrack], value[valuetrack]) !=
        2) {
      test_full += 1;
    } else {
      valuetrack++;
    }
     if (sscanf(buffer[x],
                " (%1i) (%i, %i) (%i, %i) (%i, %i) (%1i) {%[^}]} ",
                &index[curvetrack], &curves[curvetrack].p1.x,
                &curves[curvetrack].p1.y, &curves[curvetrack].p2.x,
                &curves[curvetrack].p2.y, &curves[curvetrack].p3.x,
                &curves[curvetrack].p3.y, &width[curvetrack],
                param[valuetrack]) != 9)	{
    if (sscanf(buffer[x], " (%1i) (%i, %i) (%i, %i) (%i, %i) (%1i) ",
               &index[curvetrack] ,&curves[curvetrack].p1.x,
               &curves[curvetrack].p1.y, &curves[curvetrack].p2.x,
               &curves[curvetrack].p2.y, &curves[curvetrack].p3.x,
               &curves[curvetrack].p3.y, &width[curvetrack]) != 8) {
        test_full += 1;
      } else if (curvetrack < 40) {
        curvetrack++;
      }
    }

    else if (curvetrack < 40) {
      curvetrack++;
   	  valuetrack++;
    }
  }

  for (i = 0; i < (sizeof(curves) / len); i++) {
    track_shape[i] = curves[i];
  };
  return (0);
}
