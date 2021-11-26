# Especificación de Circuitos para Half-World (HWT)

#### SPDX identifier: COIL-1.0

#### Por favor, leer HWDOC antes de este documento.
#### Los argumentos entre paréntesis son variables y deben ser reemplazados de acuerdo con como se describe en la explicación.

#### Se recomienda utilizar la extensión ".hwt" para documentos HWT (donde sea relevante).
#### HWT se puede identificar con el tipo MIME `text/hwt`


### El formato HWT:

***
```
[track]

(param) = ["(value)"]
...

[segments]

[segments.1]

shape = ["(p1x)", "(p1y)", "(p2x)", "(p2y)", "(p3x)", "(p3y)"]
width = ["(w)"]
local_param = ["(local_param)"] #param IS OPTIONAL

[segments.2]

shape = ["(p1x)", "(p1y)", "(p2x)", "(p2y)", "(p3x)", "(p3y)"]
width = ["(w)"]
```

***
EXPLICACIÓN:

Siempre empieza el archivo con la tabla 'track', seguida de flags/configuraciones, reemplazando 'param' y 'value' por el parámetro y el valor.
Después de la tabla 'track' viene la tabla 'segments'. Aquí se definen los segmentos, de 1 a un máximo de 40.

Donde todos los números con enteros, p1 y p2 son Anclas de curva de Bezier, p3 es un Control de curva de Bezier, y w es la longitud del segmento de 1 a 5.

Adicionalmente, puedes añadir propiedades especiales a ese segmento con local_param.
***

### PARÁMETROS GLOBALES DISPONIBLES:


`name = "TRCKNAME"`
#EL NOMBRE DEBE SER DIFERENTE DE '_DEFAULT'

`time = "night"/"day"`
#EL TIEMPO, DÍA/NOCHE

`biome = "desert"/"snow"/"forest"`
#EL BIOMA, DESIERTO/NIEVE/BOSQUE/ETC

`max_players = "(NUMBER)"`
#EL NUMERO DE JUGADORES MÁXIMOS

***

### SEGMENTOS DE PARÁMETRO DISPONIBLES (Incluye UNO de estos en el valor de local_param):


"boost"/"antiboost"

"finish_line"/"checkpoint"

"guardrails"/"electric_guardrails"


### EXEMPLO COGIDO DEL CIRCUITO triangular_parade.hwt
***
```
[track]
name = ["TRIANGUL"]
time = ["night"]
biome =  ["forest"]
max_players = ["6"]

[segments]

[segments.1]

shape = ["28", "36", "50", "30", "40", "40"] 
width = ["5"]
local_param = ["finish_line"]

[segments.2]

shape = ["50", "30", "37", "-50", "60", "-80"] 
width = ["3"]
local_param = ["checkpoint"]

[segments.3]

shape = ["37", "-50", "28", "36", "-50", "-80"] 
width = ["5"]
local_param = ["boost"]
```
