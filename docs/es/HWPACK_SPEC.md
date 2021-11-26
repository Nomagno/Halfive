# Formato de Paquetes para Half-World (HWPACK)

#### SPDX identifier: COIL-1.0

#### Todos los documentos HWPACK deben ser minificados (Quitar todos los espacios, tipos, y saltos de línea innecesarios).
#### Todos los documentos HWPACK deben ser codificados con ASCII NO-extendido. Por tanto, ningún character deberá ocupar más de 7 bits (+1 de acolchamiento, para completar un byte) antes de ser enviado.

#### Se recomienda utilizar la extensión ".hwpack" para documentos HWPACK (donde sea relevante).
#### HWPACK se puede identificar con el tipo MIME `text/hwpack`


### HWPACK es una versión reducida de JSON definida por la traducción de HWDOC más pequeña posible. El siguiente HWDOC:

***
```
[table]

    [table.1]
        name = ["john", "doe"]
        age = ["5", "fwive"]
        verboseage = ["fwive", "5"]
        foo = ["bar"]

    [table."1.5"]

    [table.2]
        ageofjohn = ["five", "5", "fwive"]

[thesecondtable]
	name = ["The Second"]
```
***

### Se traduce directamente al siguente HWPACK:
***
```
{
   "table": {
      "1": {
         "name": [
            "john",
            "doe"
         ],
         "age": [
            "5",
            "fwive"
         ],
         "verboseage": [
            "fwive",
            "5"
         ],
         "foo": [
            "bar"
         ]
      },
      "2": {
         "ageofjohn": [
            "five",
            "5",
            "fwive"
         ]
      },
      "1.5": {}
   },
   "thesecondtable": {
      "name": [
         "The Second"
      ]
   }
}
```
***

### Que se minifica así:

`{"table":{"1":{"name":["john","doe"],"age":["5","fwive"],"verboseage":["fwive","5"],"foo":["bar"]},"2":{"ageofjohn":["five","5","fwive"]},"1.5":{}},"thesecondtable":{"name":["The Second"]}}`

### Y esta versión puede ser transmitida eficientemente a través de cualquier red.
