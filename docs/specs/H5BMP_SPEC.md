# Halfive Bitmap Specification
#### SPDX identifier: MIT
#### Copyright Nomagno 2021, 2022, 2023

It is recommended to use the ".h5bmp" extension for H5BMP bitmaps (where applicable).
H5BMP is identified by the MIME type `application/x-h5bmp`

### H5BMP is a simple storage format for bitmaps

***
#### The H5BMP format:
It is identified by the null-terminated ASCII string 'H5BMP\0'. These six bytes shall appear at the start of every bitmap.
After the identifier string, two bytes shall follow and mark the bitmap height, and right after two bytes shall indicate the bitmap width.
Following the height and width, (width times height) two-byte pixels follow, where each string of (width) pixels represents a row, starting from the top left of the image.

#### The pixel format (RGBA 5551) in binary:
```
RRRRR GGGGG BBBBB A
```
- Where R marks the value of the colour red 0-31
- Where G marks the value of the colour green 0-31
- Where B marks the value of the colour blue 0-31
- Where A indicates whether the pixel is transparent (0) or opaque (1)
