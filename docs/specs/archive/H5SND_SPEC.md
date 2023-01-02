# Halfive Sound Specification
#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2022, 2023

H5SND is a subset of the FLAC format.

### H5SND is a Halfive-focused sound format

***
Notes

- `XX`: hex literal
- `.xxxxxxxx`: binary literal
- `.xxxxxxxx(*N)`: annotated binary literal, see annotation `(*N)` where `N` is a number
- `.lowercase(number)`: value of bit length `(number)`, see annotation `(lowercase)`

***
FILE START:
```
66 4C 61 43
80 00 00 22
.a(16) .a(16) 00 00 00 00 00 00 .b(20) .c(3) .d(5) .e(36) .f(128)
```
- a: BLOCKSIZE, in inter-channel samples, 16-bit, repeated twice
- b: Sample rate (speed at which to play back file), 20-bit, from 1Hz up to (decimal)655350Hz
- c: Number of channels, 3-bit 1 to 8
- d: BITS-PER-SAMPLE, 5-bit, 4 to 32
- e: Total number of samples, 36-bit
- f: MD5 signature of unencoded data, 128-bit, if unsure just leave as zero


***
FRAME HEADER:
```
.11111111111110(*1) .0001110000 .g(4) .0000 .h(16) .i(8)
```
- g: Number of channels, 4-bit, .0000 = mono, .0001 = stereo, rest of values forbidden
- h: BLOCKSIZE in inter-channel samples, exact same number as was used for .a(16), 16-bit
- i: CRC-8 (`polinomial = x^8 + x^2 + x^1 + x^0, initialized with 0`) of everything before the CRC up to and including the FRAME-HEADER-SYNC-LITERAL at the start of the FRAME HEADER, 8-bit
- `*1`: `.11111111111110`, called the FRAME-HEADER-SYNC-LITERAL


***
FRAME DATA:
```
.00000010 .j(BITS-PER-SAMPLE*BLOCKSIZE)
```
- j: Unencoded subblock of constant size, containing BLOCKSIZE number of samples of width BITS-PER-SAMPLE, big-endian two's complement signed integers.
- Frame data must appear only once if the value of `.g(4)` for this FRAME HEADER is `.0000`.
- Frame data must appear twice, different for the left and right channels, if the value of `.g(4)` for this FRAME HEADER is `.0001`.

***
FRAME FOOTER:
```
.k(16)
```
- k: CRC-16 (`polinomial = x^16 + x^15 + x^2 + x^0, initialized with 0`) of everything before the CRC up to and including the FRAME-HEADER-SYNC-LITERAL at the start of this FRAME HEADER, 16-bit

***
#### Repeat (FRAME HEADER, FRAME DATA, FRAME FOOTER) as many times as necessary to encode the sound data.
