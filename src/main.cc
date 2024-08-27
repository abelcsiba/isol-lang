
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <streambuf>

#include "lexer.hh"
#include "cresult.hh"
#include "util.hh"
#include <string.h>


int main(int argc, char **argv)
{
	CResult rc = SUCCESS;

	if (argc < 2)
	{
		std::cerr << "Not enough arguments!" << std::endl;
		return CResult::ERROR;
	}

	CodeFile cfile;
	cfile.name = argv[1];
	std::filesystem::path rel_dir = std::filesystem::current_path();
	cfile.location = rel_dir;

	std::ifstream source(cfile.name, std::ios::binary);
	std::string code;

	source.seekg(0, std::ios::end);   
	code.reserve(source.tellg());
	source.seekg(0, std::ios::beg);

	code.assign((std::istreambuf_iterator<char>(source)),
            std::istreambuf_iterator<char>());

	char *buffer = (char*)malloc(sizeof(char) * (code.length() + 1));
	memset(buffer, '\0', code.length() + 1);
	memcpy(buffer, code.c_str(), code.length());

	Lexer *lexer = new Lexer(buffer, cfile);

	bool verdict = lexer->lex();

	if (!verdict) 
	{

	}

	std::vector<Token> tokens = lexer->getTokens();

	for (int i = 0; i < tokens.size(); i++)
	{
		prettyPrintToken(tokens.at(i));	
	}

	return rc;
}
