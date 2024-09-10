
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

constexpr char COMPILATION_OK[] = "\n\n{0}Compilation finished {1}successfully{2} ({3}){4}";
constexpr char COMPILATION_ERROR[] = "\n\n{0}Compilation exited {1}abnormally{2} ({3}){4}";

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

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
	Diagnostics diagnostics{"", LogLevel::INFO};

	//diagnostics.warning({ .file = "main.cc", .loc = { .col = 2, .row = 3}, .msg = "This is a warning.", .other_info = "Eat shit and die"} );

	int error = manager.loadFile(cfile, argv[1]);

	if (error != 0)
	{
		exit(2);
	}

	Lexer *lexer = new Lexer(/*buffer*/ cfile.code, cfile);

	bool verdict = lexer->lex();

	if (!verdict) 
	{
		//duration = float( clock () - begin_time ) /  CLOCKS_PER_SEC;
		duration = Time::now() - begin_time;
		std::cout << std::format(COMPILATION_ERROR, WHITE, RED, WHITE, std::chrono::duration_cast<ms>(duration), RESET) << std::endl;
		exit(2);
	}

	std::vector<Token> tokens = lexer->getTokens();

	if (flag == "print")
		for (size_t i = 0; i < tokens.size(); i++)
			prettyPrintToken(tokens.at(i));

	Parser *parser = new Parser(cfile.name, std::move(tokens));

	verdict = parser->parse();

	if (!verdict) 
	{
		//duration = float( clock () - begin_time ) /  CLOCKS_PER_SEC;
		duration = Time::now() - begin_time;
		std::cout << std::format(COMPILATION_ERROR, WHITE, RED, WHITE, std::chrono::duration_cast<ms>(duration), RESET) << std::endl;
		exit(2);
	}

	duration = Time::now() - begin_time;
	std::cout << std::format(COMPILATION_OK, WHITE, GREEN, WHITE, std::chrono::duration_cast<ms>(duration), RESET) << std::endl;

	return rc;
}
