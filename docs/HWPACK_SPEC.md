# Half-World Packet Specification (HWPACK)

#### SPDX identifier: COIL-1.0

#### All HWPACK documents must be minified (Have all unecessary line breaks and spaces removed).
#### All HWPACK documents must be encoded in non-extended ASCII, and contain no non-ASCII characters. Hence, no character shall be larger than 7 bits (+1 bit of padding, to make a whole byte) before being sent.

#### It is recommended to use the ".hwpack" extension for HWPACK documents (where applicable).
#### HWPACK is identified by the MIME type `text/hwpack`

### HWPACK is a subset of JSON that is initially mapped directly from the smallest possible translation of HWDOC (TOML). The following HWDOC:

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

### Maps directly to the following HWPACK:
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

### Which is minimized like so:

`{"table":{"1":{"name":["john","doe"],"age":["5","fwive"],"verboseage":["fwive","5"],"foo":["bar"]},"2":{"ageofjohn":["five","5","fwive"]},"1.5":{}},"thesecondtable":{"name":["The Second"]}}`

### Which can then be efficiently transmitted either locally or over some kind of network.
