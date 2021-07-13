#include "tracks.c"
int main() {
  int y;
  struct Bezier track_storage[40];
  int result = get_track("../tracks/circle_madness.hwt", track_storage);
  for (y = 0; y < 40; y++) {
    printf("(%f, %f) (%f, %f) (%f, %f)\n", track_storage[y].p1.x,
           track_storage[y].p1.y, track_storage[y].p2.x, track_storage[y].p2.y,
           track_storage[y].p3.x, track_storage[y].p3.y);
  }
  printf("%d\n%s\n%s\n%s\n%s\n%s\n%s\n", result, section[0], param[0], value[0],
         section[1], param[1], value[1]);
}
