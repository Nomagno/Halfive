#include "tracks.c"

int main() {
  int y;
  char full_path[40], *path = "";
  printf("What's the name of the track file you'd like to test-parse?\n");
  scanf("%s", path);
  strcpy(full_path, "../../assets/tracks/");
  strcat(full_path, path);
  struct Track test_track = get_track(full_path);
  for (y = 0; y < 40; y++) {
  
    printf("(%i, %i) (%i, %i) (%i, %i)\n", test_track.segments[y].shape.p1.x,
           test_track.segments[y].shape.p1.y, test_track.segments[y].shape.p2.x, test_track.segments[y].shape.p2.y,
           test_track.segments[y].shape.p3.x, test_track.segments[y].shape.p3.y);
  }
  printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", "SHOW ME YOUR MOVES!", section[0], param[0], value[0],
         section[1], param[1], value[1], param[2], value[2], segment_param[0], value[3], segment_param[1], value[4]);
}
