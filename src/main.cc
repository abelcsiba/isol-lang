
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <format>
#include <chrono>

#include "lexer.hh"
#include "parser.hh"
#include "cresult.hh"
#include "file_manager.hh"
#include "diagnostics.hh"


int main(int argc, char **argv)
{
	auto begin_time = Time::now();
	fsec duration;
	CResult rc = SUCCESS;

	if (argc < 2)
	{
		std::cerr << "Not enough arguments!" << std::endl;
		return CResult::ERROR;
	}

	std::string flag = "print";

	if (argc >=3)
	{
		flag = argv[2];
	}

	CodeFile cfile;
	FileManager manager;
	Diagnostics *diagnostics = new Diagnostics("", LogLevel::INFO);

	int error = manager.loadFile(cfile, argv[1]);

	if (error != 0)
	{
		// TODO: log error
		exit(2);
	}

	Lexer *lexer = new Lexer(&cfile);

	lexer->setDiag(diagnostics);

	bool verdict = lexer->lex();

	if (!verdict) 
	{
		duration = Time::now() - begin_time;
		std::cout << std::format(COMPILATION_ERROR, WHITE, RED, WHITE, std::chrono::duration_cast<ms>(duration), RESET) << std::endl;
		exit(2);
	}

	if (flag == "print")
		for (size_t i = 0; i < cfile.tokens.size(); i++)
			prettyPrintToken(cfile.tokens.at(i));

	Parser *parser = new Parser(&cfile);
	parser->setDiag(diagnostics);

	verdict = parser->parse();

	// This should be removed when diag is finished
	if (!verdict) 
	{
		duration = Time::now() - begin_time;
		std::cout << std::format(COMPILATION_ERROR, WHITE, RED, WHITE, std::chrono::duration_cast<ms>(duration), RESET) << std::endl;
		exit(2);
	}

	duration = Time::now() - begin_time;
	std::cout << std::format(COMPILATION_OK, WHITE, GREEN, WHITE, std::chrono::duration_cast<ms>(duration), RESET) << std::endl;

	return rc;
}
