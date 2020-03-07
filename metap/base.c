
struct istream;

typedef void (*stream_reader_t)(istream *);

struct istream {
	
};

typedef union gen_tree* (*base_parser_cb)(struct base_parser *);

struct base_parser {
	base_parser_cb parser;
	char* cpt_name;
	
};
