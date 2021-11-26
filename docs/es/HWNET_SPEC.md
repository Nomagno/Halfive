# Especificación de Red para Half-World (HWNET)

#### SPDX identifier: COIL-1.0

#### Se recomienda utilizar la extensión ".hwnet" para documentos HWNET (donde sea relevante).
#### HWNET se puede identificar con el tipo MIME `text/hwnet`


#### Por favor, leer las especificaciones HWPACK y HWDOC antes de esta.
Los HWDOCs aquí representados son solo para incrementar la claridad. Todo debe ser mandado por red en un HWPACK minificado, obtenido de la conversión más compacta posible de HWDOC.

#### HWNET está diseñada para funcionar con una capa de transporte capaz de transportar 400 bytes, con *cualquier* cantidad de pérdida de paquetes/inestabilidad. Debido a esto, no realiza verificación de identidad del jugador, pero se puede extender fácilmente para hacer esto. *Debería* ser imposible mandar un paquete que se adhiera a la especificación que tenga una longitud de más de 400 caracteres ASCII.

### Cuando se solicita informatión del Nodo Servidor, el siguiente formato debería seguirse:

### FORMATO NODO CLIENTE
```
[user]
id=["(identification)" ...]

[data]
	exten=["(ext)"]
	[data.(identification)]
		thrusters=["(thru1)", "(thru2)", "(steer)"]

[query]
	mode=["(mode)"]
```
Donde (identification) es el número(s) de jugador deseados para controlar, sin embargo no se deben solicitar más de cuatro distintos..
Donde (ext) es la especificación deseada por el Cliente. El Cliente debería estar preparado para cambiar al formato base si no consigue entender las respuestas del Nodo Servidor.

Donde (thru1) es el valor del primer eje de pedal
Donde (thru2) es el valor del segundo eje de pedal
Donde (steer) es el valor del eje del volante

Donde (mode) es una de las letras específicadas justo debajo:

### CLIENT NODE MODES

r - RACE INFORMATION
l - LEADERBOARD INFORMATION
***

#### El servidor debe responder a la dirección IP del cliente (o el equivalente más cercano en caso el protocolo usado no es IP) siguiendo este formato:
### MASTER NODE FORMT
```
[data]
track=["(trackname)"]
game=["(gamemode)"]
ext=["(extension)"]
1=["(pos1)", "(pos2)", "t1", "t2", "t3"]
2=["(pos1)", "(pos2)", "t1", "t2", "t3"]
3=["(pos1)", "(pos2)", "t1", "t2", "t3"]
4=["(pos1)", "(pos2)", "t1", "t2", "t3"]
5=["(pos1)", "(pos2)", "t1", "t2", "t3"]
6=["(pos1)", "(pos2)", "t1", "t2", "t3"]
7=["(pos1)", "(pos2)", "t1", "t2", "t3"]
8=["(pos1)", "(pos2)", "t1", "t2", "t3"]
```
***
Donde (trackname) es un set de ocho caracteres alfanuméricos que representan el Circuito usado.
Donde (gamemode) es un set de ocho caracteres alfanuméricos que representan el Modo usado. Para indicar una carrera normal, debería ser "_DEFAULT"

Donde (pos1) y (pos2) son una representación 2D de ese vehículo, y t1-t3 son las flags descritar debajo. 

Nota: Estas flags son abstractas y le dan contexto al cliente, sin dar indicaciones computacionalmente específicas. Si la flag 'r' es recibida, Tipo Carrera 1, 2, y 3 deben ser proporcionados. Si la flag 'l' es recibida, Tipo Tabla de Puntuaciones 1 debe ser proporcionado, y los otros dos deberan ser dejados como strings vacías.

### FLAGS DE NODO SERVIDOR

### SOLICITACIÓN AL SERVIDOR

r - CARRERA
l - TABLA DE PUNTUACIONES

### Estado de carrera ["" "" ""]

- TIPO 1 - ESTADOS DEL VEHÍCULO -
o - FUERA DE CARRETERA
u - DERRAPE
x - HERIDO
s - LENTO
z - RÁPIDO
e - ELECTRIFICADO
c - CHOQUE
y - NADA

- TIPO 2 - ESTADOS DEL MOVIMIENTO -
f - HACIA DELANTE
b - HACIA ATRÁS
k - ARRANQUE
i - PARADO

- TIPO 3 - ESTADOS DE LA CONEXIÓN -
a - CONECTADO
m - CONEXIÓN MÚLTIPLE
t - DESCONEXIÓN
h - PAUSA
***
### ESTADOS DE LA TABLA DE PUNTUACIONES TIPO 4

(1..8) - POSICIÓN
***

EJEMPLO DE PAQUETE DEL CLIENTE

`{"user":{"id":["1","2","3","4"]},"data":{"1":{"thrusters":["123567","1234567","1234567"]},"2":{"thrusters":["123567","1234567","1234567"]},"3":{"thrusters":["123567","1234567","1234567"]},"4":{"thrusters":["123567","1234567","1234567"]},"exten":["HalfWrld"]},"query":{"mode":["r"]}}`


EJEMPLO DE PAQUETE DEL SERVIDOR

`{"data":{"1":["1234567","1234567","a","b","c"],"2":["1234567","1234567","a","b","c"],"3":["1234567","1234567","a","b","c"],"4":["1234567","1234567","a","b","c"],"5":["1234567","1234567","a","b","c"],"6":["1234567","1234567","a","b","c"],"7":["1234567","1234567","a","b","c"],"8":["1234567","1234567","a","b","c"],"track":["TRACKADD"],"game":["RACERACE"],"ext":["HelloIts"]}}`
