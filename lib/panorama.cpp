#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <opencv2/opencv.hpp>
#include "ImageStitcher.h"
#include "RansacMatcher.h"

int safemain( int argc, char *argv[] )
{
	
	// step 0: parse arguments
	// needed arguments
	std::string inPath;       // -i <arg>
	int frameStart, frameEnd; // -f <arg1> <arg2>
	int resizeScale;          // -r <arg>
	int distance;             // -d <arg>

	std::map<std::string, int> checklist;
	checklist["-i"] = 1;
	checklist["-f"] = 2;
	checklist["-r"] = 1;
	checklist["-d"] = 1;

	for ( size_t i = 1; i < argc; ++i )
	{
		if ( checklist.find( argv[i] ) != checklist.end() )
		{
			if ( strcmp( argv[i], "-i" ) == 0 )
			{
				inPath = std::string( argv[++i] );
			}

			if ( strcmp( argv[i], "-f" ) == 0 )
			{
				frameStart = atoi( argv[++i] );
				frameEnd = atoi( argv[++i] );
			}
			
			if ( strcmp( argv[i], "-r" ) == 0 )
			{
				resizeScale = atoi( argv[++i] );
			}

			if ( strcmp( argv[i], "-d" ) == 0 )
			{
				distance = atoi( argv[++i] );
			}
		}
	}

	ImageStitcher stitcher( inPath, frameStart, frameEnd, resizeScale, distance );
	stitcher.display();
	int frameNum = frameEnd - frameStart + 1;

	// step 1: read data, resize
	std::vector<cv::Mat> images;
	stitcher.readImages( images );
	stitcher.resizeImages( images );
	
	// step 2: warp images
	cv::Mat mask;
	stitcher.projectImagesToCylinder( images, mask );

	// step 3: feature detection, matching, homography estimation
	std::vector<cv::Mat> homographies;
	stitcher.estimateHomographies( images, homographies );

	// step 4: stitch images
	cv::Size canvas_size( -1, -1 );
	cv::Mat result;
	stitcher.stitchImages( images, mask, homographies, result );

	// step 5: write the results
	cv::namedWindow( "Panorama", cv::WINDOW_AUTOSIZE );
	cv::imshow( "Panorama", result );
	cv::waitKey( 0 );

	cv::imwrite( "/tmp/result.png", result );
	// std::cout << "writing result image: " << std::string( src ) + std::string( "/out/result.jpg" ) << std::endl;
 //    imwrite( string(src) + string( "/out/result.jpg" ), result );


	return 0;
}

int main( int argc, char *argv[] )
{	
	try
	{
		return safemain( argc, argv );
	}
	catch( std::exception &e )
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return -1;

}


