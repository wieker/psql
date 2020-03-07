
void* parse_grammar();
extern char* input;
void gen_grammar(void*);

int
main(int argc, char** argv)
{
	input = "ququ = dfdf ffqq dduu\n"
			"sdsdsdsd;"
			"dfddd = rtrt uu / rtrtrt gg / fff ddd;";
	gen_grammar(parse_grammar());
	return 0;
}
