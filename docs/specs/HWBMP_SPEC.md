# Half-World Bitmap Specification
#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2021, 2022

It is recommended to use the ".hwbmp" extension for HWBMP bitmaps (where applicable).
HWBMP is identified by the MIME type `application/hwbmp`

### HWBMP is a simple storage format for bitmaps

***
#### The HWBMP format:
It is identified by the null-terminated ASCII string 'HWBMP\0'. These six bytes shall appear at the start of every bitmap.
After the identifier string, two bytes shall follow and mark the bitmap width, and right after two bytes shall indicate the bitmap width.
Following the width and height, (width times height) two-byte pixels follow, where each string of (width) pixels represents a row, starting from the top left of the image.

#### The pixel format (RGBA 5551) in binary:
```
RRRRR GGGGG BBBBB A
```
- Where R marks the value of the color red 0-31
- Where G marks the value of the color green 0-31
- Where B marks the value of the color blue 0-31
- Where A indicates whether the pixel is transparent (0) or opaque (1)
