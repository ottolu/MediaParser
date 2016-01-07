#include "Stdafx.h"

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include <ctime>

#include "Config.h"
#include "Parser.h"

int main(int argc, char *argv[])
{
	//char filename[20];

	//if( argc != 2 )
	//{
	//	// std::cout << "Usage: " << argv[ 0 ] << " FILENAME\n";
	//	std::cout << "Please input the mpeg4 file name:\n";
	//	std::cin >> filename;
	//}
	//else
	//{
	//	strcpy_s(filename, argv[1]);
	//}

	clock_t time = std::clock();

	MPEG4Parser::ConfigInit();
	
	std::shared_ptr<MediaParser::MP4Parser> mp4parser;

	mp4parser = std::shared_ptr<MediaParser::MP4Parser>(
		new MediaParser::MP4Parser("D:\\test\\test2.mp4"));
	//mp4parser = std::shared_ptr<MediaParser::MP4Parser>(new MediaParser::MP4Parser(filename));

	mp4parser->parse();

	std::cout << (std::clock() - time) << std::endl;

	mp4parser->mediaSplite();
    
	//_CrtDumpMemoryLeaks();

	system("pause");

    return EXIT_SUCCESS;
}