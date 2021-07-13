#include <stdio.h>
#include <string.h>
struct Point {
  float x;
  float y;
};
struct Bezier {
  struct Point p1;
  struct Point p2;
  struct Point p3;
};
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

    if (sscanf(buffer[x], " (%1i) (%f, %f) (%f, %f) (%f, %f) (%1i) ",
               &index[curvetrack], &curves[curvetrack].p1.x,
               &curves[curvetrack].p1.y, &curves[curvetrack].p2.x,
               &curves[curvetrack].p2.y, &curves[curvetrack].p3.x,
               &curves[curvetrack].p3.y, &width[curvetrack]) != 8) {
      if (sscanf(buffer[x],
                 " (%1i) (%f, %f) (%f, %f) (%f, %f) (%1i) + %s = %s ",
                 &index[curvetrack], &curves[curvetrack].p1.x,
                 &curves[curvetrack].p1.y, &curves[curvetrack].p2.x,
                 &curves[curvetrack].p2.y, &curves[curvetrack].p3.x,
                 &curves[curvetrack].p3.y, &width[curvetrack],
                 param[valuetrack], value[valuetrack]) != 10) {
        test_full += 1;
      } else if (curvetrack < 40) {
        curvetrack++;
        valuetrack++;
      }
    }

    else if (curvetrack < 40) {
      curvetrack++;
    }
  }

  for (i = 0; i < (sizeof(curves) / len); i++) {
    track_shape[i] = curves[i];
  };
  // test_full = test1 + test2 + test3 + test4 + test5;
  return (0);
}
