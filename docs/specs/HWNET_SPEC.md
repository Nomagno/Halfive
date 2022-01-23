# Half-World Network Specification
#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2021, 2022

HWNET is designed to work in any transport layer that is capable of handling packet payloads of up to 400 bytes, with *any* amount of packet loss and connection instability. As such, it performs no verification of player identity, but it is fairly trivial to extend it to do such a thing. It *should* be impossible to write a compliant, sane packet that exceeds 400 ASCII characters/bytes in length, however there is plenty of room to expand until the more realistic 1000+ bytes that most protocols can handle, which leaves rooms for extensions to touch this specification.

It is recommended to use the ".hwnet" extension for HWNET documents (where applicable).
HWNET is identified by the MIME type `text/hwnet`

### HWNET is a specification for racing game communication
***
When querying the node that is acting as the server or 'MASTER', the following format should be followed:

#### CLIENT NODE FORMAT:

```
_USER
	id (identification) ...

_DATA
	exten (ext)
	_DATA_(id)
		thrusters (thru1) (thru2) (steer)

_QUERY
	mode (mode)
```
***

Where (identification) is the desired player number(s) to control, however the client node may not request more than 4.
where (ext) is the extension the client wants to use. The client should be prepared to roll back to basic 'HalfWrld' if the server does not reply in the expected format of the extension.

Where (thru1) is the value of the first thruster axis
Where (thru2) is the value of the second thruster axis
Where (steer) is the value of the steering axis

Where (mode) is one of the letters described below

#### CLIENT NODE MODES:
r - RACE INFORMATION
l - LEADERBOARD INFORMATION

The server should reply to the IP address (or closest equivalent at the implementation's discretion, in case the protocol used is not IP) following this template:
***

#### MASTER NODE FORMAT:

```
_DATA
	track (trackname)
	game (gamemode)
	ext (extension)
	1 (pos1) (pos2) (t1) (t2) (t3)
	2 (pos1) (pos2) (t1) (t2) (t3)
	3 (pos1) (pos2) (t1) (t2) (t3)
	4 (pos1) (pos2) (t1) (t2) (t3)
	6 (pos1) (pos2) (t1) (t2) (t3)
	7 (pos1) (pos2) (t1) (t2) (t3)
	8 (pos1) (pos2) (t1) (t2) (t3)
```
***

Where (trackname) is an eight character alphanumerical string that represents the track being used.
Where (gamemode) is an eight character alphanumerical string that represents the gamemode being used. To indicate a normal race mode, it should be "_DEFAULT"

Where (pos1) and (pos2) are a 2D representation of that player's vehicle position, and t1-3 are the flags described below.

Note these flags are abstract and meant to give the client context, as opposed to any kind of computationally-specific indications. If the flag 'r' is received as the client packet mode, race type 1, race type 2, and race type 3 flags shall be supplied. If the flag 'l' is received, the leaderboard information should be supplied at (t1) flag, and the other two types should not be included.

#### MASTER NODE FLAGS:
***
#### GAME STATE QUERY

r - RACE
l - LEADERBOARD
***

```
RACE STATES

- TYPE 1 - VEHICLE STATES -
o - OFFTRACK
u - DRIFT
x - HURT
s - SLOWER
z - FASTER
e - ELECTRIFIED
c - CRASHED
y - NONE

- TYPE 2 - MOVEMENT STATES -
f - FORWARDS
b - BACKWARDS
k - STARTUP
i - STOPPED

- TYPE 3 - CONNECTION STATES -
a - CONNECTED
m - MULTIPLEX
t - TIMEOUT
h - PAUSE
```
1
#### LEADERBOARD STATES TYPE 1

(1..8) - POSITION
