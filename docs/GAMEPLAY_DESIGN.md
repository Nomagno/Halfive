# Half-World gameplay design document

## Mechanics

### CAR CONTROLS
The player controls a hovercar with a bumper car-like frame that has two backside/underside thrusters/suction devices.
The HV is controlled with a lever for each thruster ("Lever" can be any analog onedimensional input method).
These levers can control how much power each of the thrusters receive:
Pull right lever, HV turns right.
Pull left lever, HV turns left.
Pull both levers, HV accelerates in a straight line.
Right and left levers can be pulled to any arbitrary position to control rotation and speed.

### TRACK FEATURES
The track has certain intrinsic global properties:

- The biome it is in (Does not affect gameplay).
- The time of the day (Does not affect gameplay).
- The maximum number of players.

The track segments are defined by quadratic (Three-point) Bezier curves, in addition to an index and a width from 1 to 10. Each segment can have extra properties:

- Finish line / Control point (For reappearing when going off track)
- Guard rails / Electric guard rails
- Slower speed (Track is dirty/unpaved) / Faster speed (Track has air suction devices on the floor)
