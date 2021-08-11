//DEPRECATED, AWAITING A HWDOC-COMPLIANT REIMPLEMENTATION

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
enum SPEED { normal, boost, antiboost };
enum RACING_LINE { no, finish_line, checkpoint };

enum GUARDRAILS { empty, metal, electric };

enum BIOME { forest, desert, snow };

enum COLORS { white, red, blue, yellow, green, pink };

struct Biome {
  char spritesheet[100];
  enum COLORS main_color;
  enum BIOME the_biome;
};

enum TIME { unknown, day, night };

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
  int index;

  struct Bezier shape;

  enum SPEED speed;
  enum RACING_LINE racing_line;
  enum GUARDRAILS guardrails;
};

struct Track {
  char name[20];

  struct Biome biome;
  enum TIME time;
  int max_players;

  struct Segment segments[40];
};

struct Track tracks[10];
struct Bezier curves[40];

char section[10][20], param[10][20], value[10][20], segment_param[10][20];
char buffer[80][100];

// To calculate point [t] of the Bezier curve:
// A*(1-t)^2 + B*2t(1-t) + C*t^2

struct Track get_track(char *track_file) {

  FILE *fp;
  struct Track read_track;
  int i = 0, x = 0, curvetrack = 0, sectiontrack = 0, valuetrack = 0;
  int index[40], width[40], test_full = 0;
  for (x = 0; x < 10; x++) {
    strcpy(section[x], "");
    strcpy(param[x], "");
    strcpy(value[x], "");
  }
  for (x = 0; x < 80; x++) {
    strcpy(buffer[x], "");
  }
  for (x = 0; x < 40; x++) {
    strcpy(segment_param[x], "");
    index[x] = 0;
  }

  fp = fopen(track_file, "r");

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
    if (sscanf(buffer[x], " (%1i) (%i, %i) (%i, %i) (%i, %i) (%1i) {%[^}]} ",
               &index[curvetrack], &curves[curvetrack].p1.x,
               &curves[curvetrack].p1.y, &curves[curvetrack].p2.x,
               &curves[curvetrack].p2.y, &curves[curvetrack].p3.x,
               &curves[curvetrack].p3.y, &width[curvetrack],
               segment_param[curvetrack]) != 9) {
      if (sscanf(buffer[x], " (%1i) (%i, %i) (%i, %i) (%i, %i) (%1i) ",
                 &index[curvetrack], &curves[curvetrack].p1.x,
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

    }
  }

  for (i = 0; i < 10; i++) {

    if (strcmp(param[i], "TIME") == 0) {
      if (strcmp(value[i], "NIGHT") == 0) {
        read_track.time = night;
      }

      else if (strcmp(value[i], "DAY") == 0) {
        read_track.time = day;
      }

      else {
        read_track.time = unknown;
      }
    }

    if (strcmp(param[i], "BIOME") == 0) {

      if (strcmp(value[i], "DESERT") == 0) {
        read_track.biome.the_biome = desert;
      }

      else if (strcmp(value[i], "SNOW") == 0) {
        read_track.biome.the_biome = snow;
      }

      else {
        read_track.biome.the_biome = forest;
      }
    }

    if (strcmp(param[i], "MAX_PLAYERS") == 0) {
      read_track.max_players = strtol(value[i], NULL, 10);
    }
  }

  for (i = 0; i < 40; i++) {
    if (strcmp(segment_param[i], "BOOST") == 0) {
      read_track.segments[i].speed = boost;
    } else if (strcmp(segment_param[i], "ANTIBOOST") == 0) {
      read_track.segments[i].speed = antiboost;
    } else {
      read_track.segments[i].speed = normal;
    }

    if (strcmp(segment_param[i], "GUARDRAILS") == 0) {
      read_track.segments[i].guardrails = metal;
    } else if (strcmp(segment_param[i], "ELECTRIC_GUARDRAILS") == 0) {
      read_track.segments[i].guardrails = electric;
    } else {
      read_track.segments[i].guardrails = empty;
    }

    if (strcmp(segment_param[i], "RACING_LINE") == 0) {
      read_track.segments[i].racing_line = finish_line;
    } else if (strcmp(segment_param[i], "CHECKPOINT") == 0) {
      read_track.segments[i].racing_line = checkpoint;
    } else {
      read_track.segments[i].racing_line = no;
    }
    read_track.segments[i].shape = curves[i];
    read_track.segments[i].index = index[i];
  }
  return read_track;
}
