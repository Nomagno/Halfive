# Half-World Track (Encoding) Specification

#### SPDX identifier: COIL-1.0
#### Please read the HWDOC format specification before this one.
#### Arguments between parentheses are variable and should be replaced according to their respective explanations.

#### It is recommended to use the ".hwt" extension for HWT documents (where applicable).
#### HWT is identified by the MIME type `text/hwt`

### The HWT format:

***
```
_TRACK
	(param) (value)
	...

_SEGMENTS
	_SEGMENTS_1
		shape (p1x) (p1y) (p2x) (p2y) (p3x) (p3y)
		width (w)
		local_param (local_param)
	_SEGMENTS_2
		shape (p1x) (p1y) (p2x) (p2y) (p3x) (p3y)
		width (w)
	...
```
***

### EXPLANATION:

Always start the file with the track section, followed by flags/settings, of course replacing param and value for the parameter and the value.
After the track table comes the segments table. Here you define the segments, from one to a maximum of 40.

Where all numbers are integers, p1 and p2 are bezier curve anchors, p3 is a bezier curve control point, and w is the track width from 1 to 5.

Aditionally, you can add special properties for that segment with local_param.


***

AVAILABLE GLOBAL PARAMETERS:

`name trckname`

#THE NAME HAS TO BE AN EIGHT CHARACTER ALPHANUMERICAL STRING THAT IS NOT '_DEFAULT'

`time night/day`

`biome = desert/snow/forest`
#AN ADDITIONAL "color - white/red/blue/yellow/green/pink" PARAMETER WILl BE GENERATED FROM THE BIOME. IF YOU ONLY CARE ABOUT THE COLOR OF THE STAGE/TRACK BACKGROUND, PLEASE STILL SPECIFY A BIOME

`max_players (NUMBER)`


***
### AVAILABLE SEGMENT PARAMETERS (Include ONE of these in the local_param value):


boost/antiboost

finish_line/checkpoint

guardrails/electric_guardrails


### EXAMPLE DRAWN FROM THE triangular_parade.hwt TRACK
***

```
_TRACK
	time night
	biome forest
	max_players 6

_SEGMENTS
	_SEGMENTS_1
		shape 28 36 50 30 40 40
		width 5
		local_param finish_line
	_SEGMENTS_2
		shape 50 30 37 -50 60 -80
		width 3
		local_param checkpoint
	_SEGMENTS_3
		shape 37 -50 28 36 -50 -80
		width 5
		local_param boost
```
