# Half-World Document Specification (The encoding format of HWT)
#### SPDX identifier: COIL-1.0
#### HWDOC is a subset of TOML 1.0

#### It is recommended to use the ".hwdoc" extension for HWDOC documents (where applicable).
#### HWDOC is identified by the MIME type `text/hwdoc`

***
### The only supported features: 
	- Tables 
	- Key-Value pairs 
	- Values as arrays of strings 
	- One level of subtables 
	- Comments 

### Length limitations (Quotes that aren't part of the names don't count):
	- Arrays | 20 elements
	- Strings | 20 chars
	- Key names | 20 chars
	- Table names  (Not including subtables) | 20 chars
	- Table names (Including subtables) | 41 chars
	- Comments | UNLIMITED

***

### Everything else is officially unsupported. Examples:
	- Numbers
	- Strings not inside arrays
	- The top-level table does not exist
	- Level 2 Nested tables
	- Arrays of tables
	- Nested arrays
	- Referencing
	
#### Example of what to do below

```
#PRETTY MUCH ONLY STUFF SEEN HERE IS VALID

[table]

    [table.1]
        name = ["john", "doe"]
        age = ["5", "fwive"]
        verboseage = ["fwive", "5"]
        foo = ["bar"]

    [table."1.5"] 
    #Empty Subtable named 1.5

    [table.2]
        ageofjohn = ["five", "5", "fwive"]

[thesecondtable]
	name = ["The Second"]
```
#### Example of what NOT to do below

```
#ALL OF THIS IS INVALID

#THERE IS NO TOP LEVEL TABLE
a="t" #ONLY ARRAYS OF STRINGS ARE SUPPORTED
a=1 #ONLY ARRAYS OF STRINGS ARE SUPPORTED
a=[1,2] #ONLY ARRAYS OF STRINGS ARE SUPPORTED

    [table.1] #THERE IS NO IMPLICIT TABLE CREATION
        verboseage = ["fwive", ["4", "5"]] #NESTED ARRAYS ARE NOT SUPPORTED
        "twentycharacterslong" = ["twentyandsixcharacterslong"] #MAX NAME CHARACTER LENGTH IS 20

    [table.1.5] #LEVEL TWO SUBTABLES ARE NOT SUPPORTED 

        ageofjohn = 5.0001 #ONLY ARRAYS OF STRINGS ARE SUPPORTED
```
