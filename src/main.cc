
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

#include "lexer.hh"
#include "parser.hh"
#include "cresult.hh"
#include "file_manager.hh"


int main(int argc, char **argv)
{
	CResult rc = SUCCESS;

	if (argc < 2)
	{
		std::cerr << "Not enough arguments!" << std::endl;
		return CResult::ERROR;
	}

	CodeFile cfile;
	FileManager manager;

	int error = manager.loadFile(cfile, argv[1]);

	if (error != 0)
	{
		exit(2);
	}

	Lexer *lexer = new Lexer(/*buffer*/ cfile.code, cfile);

	bool verdict = lexer->lex();

	if (!verdict) 
	{

	}

	std::vector<Token> tokens = lexer->getTokens();

	for (size_t i = 0; i < tokens.size(); i++)
	{
		prettyPrintToken(tokens.at(i));	
	}

	Parser *parser = new Parser(std::move(tokens));

	verdict = parser->parse();

	std::cout << (verdict ? "true" : "false") << std::endl;

	std::cout << "Module: " << parser->getModule()->module_name << std::endl;

	for (auto dep : parser->getModule()->dependencies)
	{
		std:: cout << " - " << dep << std::endl;
	}

	return rc;
}
