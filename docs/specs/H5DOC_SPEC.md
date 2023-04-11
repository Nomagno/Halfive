# Halfive Document Specification (The encoding format of H5T and H5NET)
#### SPDX identifier: MIT
#### Copyright Nomagno 2021, 2022, 2023

It is recommended to use the ".h5doc" extension for H5DOC documents (where applicable).
H5DOC is identified by the MIME type `text/x-h5doc`

### H5DOC is a simple storage format for slightly nested data

***

#### The relationships between data are determined by three bytes:
 - Values after a key are dictated by a simple space, `' '` (ASCII code 32, byte 0x20)
 - End of a section name or key-value pair is dictated by a newline, `'\n'` (ASCII code 10, byte 0x0A)
 - A child of a section (can be a key-value pair or another section) is dictated by a horizontal tab, `'\t'` (ASCII code 9, byte 0x09)
 - These three bytes are to be understood as "whitespace characters"

#### The types of elements are determined by these rules:
 - Sections, strings that do not contain whitespace, prefixed with underscore `'_'` (ASCII code 95, byte 0x5F) e.g. "_My_SECTION1"
 - Keys, strings that do not contain whitespace, NOT prefixed with underscore e.g. "My_Key1"
 - Values, strings located after a key (separated by a simple space, 0x20), e.g. "mykey value1 value2"

***

#### Example:

```
_SUPER_SECTION
	first_key 1 6 !k
	_SUB_SECTION
		second_key
_SECOND_SUPER_SECTION
```

Same document, but encoded with newline marked as `'\n'` and horizontal tab marked as `'\t'`:

```
_SUPER_SECTION\n\tfirst_key 1 6 !k\n\t_SUB_SECTION\n\t\tsecond_key\n_SECOND_SUPER_SECTION
```

'_SUPER_SECTION' is a section that contains a key 'first_key', (with values '1', '2' and '!k') and another section '_SUB_SECTION', which contains a key without values 'second_key'. After '_SUPER_SECTION', there is '_SECOND_SUPER_SECTION', an empty section
