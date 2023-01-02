# Halfive Track (Encoding) Specification

#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2021, 2022
#### Please read the H5DOC format specification before this one.
NOTE: Arguments between parentheses are variable and should be replaced according to their respective explanations.

It is recommended to use the ".h5t" extension for H5T documents (where applicable).
H5T is identified by the MIME type `text/h5t`

### H5T is a format for storing racing game tracks
***
```
_TRACK
	(param) (value)
	...

_SEGMENTS
	_SEGMENTS_1
		shape_outer (p1x) (p1y) (p2x) (p2y) (p3x) (p3y)
		shape_inner (p1x) (p1y) (p2x) (p2y) (p3x) (p3y)
		local_param (local_param)
	_SEGMENTS_2
		shape_outer (p1x) (p1y) (p2x) (p2y) (p3x) (p3y)
		shape_inner (p1x) (p1y) (p2x) (p2y) (p3x) (p3y)

	...
```
***

### EXPLANATION:

Always start the file with the track section, followed by flags/settings, of course replacing param and value for the parameter and the value.
After the track table comes the segments table. Here you define the segments, from one to a maximum of 40.

Where all numbers are unsigned integers, p1 and p3 are bezier curve anchors, p2 is a bezier curve control point.
pNx and pNy are unsigned 32-bit integers, and their unit is millimetres
'shape_outer' describes the outer outline of the track segment
'shape_inner' describes the inner outline of the track segment

Aditionally, you can add special properties for that segment with local_param.


***

#### AVAILABLE GLOBAL PARAMETERS:

`name trckname`

The name must be an eight character alphanumerical string that is not '_DEFAULT'

`time night/day`

`biome = snow/lava/lake/desert/forest/rosegarden`

An additional "colour - while/red/blue/yellow/green/pink" parameter will be generated from the biome.

`max_players (NUMBER)`


***

#### AVAILABLE SEGMENT PARAMETERS (Include ONE of each pair max in the local_param value):


boost/antiboost

finish_line/checkpoint

guardrails/electric_guardrails
