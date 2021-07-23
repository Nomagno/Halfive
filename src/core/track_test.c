#include "tracks.c"

int main() {
  int y;
  struct Bezier track_storage[40];
  char full_path[40], *path;
  printf("What's the name of the track file you'd like to test-parse?\n");
  scanf("%s", path);
  strcpy(full_path, "../../assets/tracks/");
  strcat(full_path, path);
  int result = get_track(full_path, track_storage);
  for (y = 0; y < 40; y++) {
    printf("(%i, %i) (%i, %i) (%i, %i)\n", track_storage[y].p1.x,
           track_storage[y].p1.y, track_storage[y].p2.x, track_storage[y].p2.y,
           track_storage[y].p3.x, track_storage[y].p3.y);
  }
  printf("%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", result, section[0], param[0], value[0],
         section[1], param[1], value[1], param[2], value[2], param[3], value[3], param[4], value[4]);
}
