typedef struct{
	char *str;
} value;

typedef struct{
	char *str;
	value *vals;
} key;

typedef struct{
	char *str;
	key *keys;
} section;

typedef struct{
	char *name;
	section *sections;
} hwdoc;

extern hwdoc hwdoc_parse(char*);
