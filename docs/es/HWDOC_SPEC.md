# Formato de Documentos para Half-World (HWDOC)

#### SPDX identifier: COIL-1.0

#### HWDOC es una versión reducida de TOML 1.0
#### Se recomienda utilizar la extensión ".hwdoc" para documentos HWDOC (donde sea relevante).
#### HWDOC se puede identificar con el tipo MIME `text/hwdoc`


***
### Todo lo soportado: 
	- Tables
	- Pares llave-valor
	- Valores como arrays de strings
	- One level of subtables
	- Comentarios

### Limitaciones de longitud (Comillas que no sean parte de los nombres no cuentan):
	- Arrays | 20 elementos
	- Strings | 20 characteres
	- Nombres de llave | 20 characteres
	- Nombres de tabla (Sin incluir tablas incrustadas) | 20 characteres
	- Table names (Incluyendo tablas incrustadas) | 41 characteres
	- Commentarios | ILIMITADO

***
### Todo lo demás no está soportado. Ejemplos:
	- Números
	- Strings que no estén dentro de arrays
	- La table superior no existe
	- Tablas encrustadas más de dos niveles
	- Arrays de tablas
	- Arrays incrustadas
	- Referencias

### Ejemplo de que hacer debajo

```
#PRÁCTICAMENTE SOLO LAS COSAS QUE SE HACEN AQUÍ SON VÁLIDAS

[table]

    [table.1]
        name = ["john", "doe"]
        age = ["5", "fwive"]
        verboseage = ["fwive", "5"]
        foo = ["bar"]

    [table."1.5"] 
    #Tabla vacía con nombre 1.5

    [table.2]
        ageofjohn = ["five", "5", "fwive"]

[thesecondtable]
	name = ["The Second"]
```

### Ejemplo de que NO hacer debajo
```
#TODO LO QUE VES AQUÍ ES INVÁLIDO

#NO HAY TABLA SUPERIOR
a="t" #SOLO ARRAYS DE STRINGS SON VÁLIDAS
a=1 #SOLO ARRAYS DE STRINGS SON VÁLIDAS
a=[1,2] #SOLO ARRAYS DE STRINGS SON VÁLIDAS

    [table.1] #NO HAY CREACIÓN DE TABLAS IMPLÍCITA
        verboseage = ["fwive", ["4", "5"]] #LAS ARRAYS INCRUSTADAS SON INVÁLIDAS
        "twentycharacterslong" = ["twentyandsixcharacterslong"] #LA LONGITUD MÁXIMA DE NOMBRES ES 20 characteres

    [table.1.5] #NO SE PUEDE INCRUSTAR UNA TABLA DOS NIVELES

        ageofjohn = 5.0001 #SOLO ARRAYS DE STRINGS SON VÁLIDAS
```
