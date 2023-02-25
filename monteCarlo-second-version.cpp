#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

using namespace std;

typedef struct {
	int top;
	int left;
	int right;
	int bottom;
} Rectangle;

Rectangle sorroundingShape;

typedef struct {
	int x;
	int y;
} Point;

int isPointInsideTheCurve(Point point) {

	double x_square = point.x * point.x;
	double y_square = point.y * point.y;
	double position = (x_square / 10000) + (y_square / 2500) - 1;
	if (position <= 0.0) {
		return true;
	} else {
		return false;
	}	
}

Point generateRandomPointInsideBox(Rectangle box, unsigned int *seed) {

	
	int horizontal = (box.right - box.left);
	int vertical = (box.top - box.bottom);

	int x = (rand_r(seed) % horizontal) + box.left;
        int y = (rand_r(seed) % vertical) + box.bottom;

	// generate and return the point
	Point randomPoint;
	randomPoint.x = x;
	randomPoint.y = y;
	return randomPoint;
}

double estimateArea(Rectangle boundingBox, int sampleCount) {

	int mainThreadId = 0;
	unsigned int threadIdSeed = mainThreadId;

	// initialize the area estimate variable;
	double estimate = 0.0;
	// also initialize a variable to count the number of points we found to be inside the curve
	int insidePoints = 0;

	// doing iteration count number of sampling experiments
	for (int i = 0; i < sampleCount; i++) {
		Point randomPoint = generateRandomPointInsideBox(boundingBox, &threadIdSeed);
		if (isPointInsideTheCurve(randomPoint)) {
			insidePoints++;
		}
	}
		
	std::cout << "total points inside: " << insidePoints << "\n";

	// estimate the area under the curve from the area of the bounding box
	double boxArea = (boundingBox.right - boundingBox.left) * (boundingBox.top - boundingBox.bottom);
	double curveArea = (boxArea * insidePoints) / sampleCount;

	// return the area estimate for the curve
	return curveArea;
}

int THREAD_COUNT = 8;	 

typedef struct {
	int threadId;

} ThreadArg;


void *threadFunction(void *arg) {
	
	ThreadArg *argument = (ThreadArg *) arg;
	int threadId = argument->threadId;
	cout << "Thread " << threadId << ": says hello\n";
	return NULL;
}

int main(int argc, char *argv[]) {
	
	if (argc < 6) {
		std::cout << "To run the program you have to provide two things.\n";
		std::cout << "\tFirst the number of samples for the Monte Carlo sampling experiments.\n";
		std::cout << "\tSecond the bounding area within which the samples should be generated.\n";
		std::cout << "The format of using the program:\n";
		std::cout << "\t./program_name sample_count bottom_left_x, bottom_left_y, top_right_x, top_right_y\n";
		std::exit(EXIT_FAILURE);
	}

	int sampleCount = atoi(argv[1]);

	// assigning the input parameters to the sorrounding shape object; we are interested in finding the area of the part of the 
	// curve that is inside this shape.
	sorroundingShape.left = atoi(argv[2]);
	sorroundingShape.bottom = atoi(argv[3]);
	sorroundingShape.right = atoi(argv[4]);
	sorroundingShape.top = atoi(argv[5]);


	// starting execution timer clock
        struct timeval start;
        gettimeofday(&start, NULL);
	
	// initialize the random number generator;
	srand(time(NULL));

	pthread_t *threads;
	ThreadArg *threadArgs;
	threads = (pthread_t *) malloc(THREAD_COUNT * sizeof(pthread_t));
	threadArgs = (ThreadArg *) malloc(THREAD_COUNT * sizeof(ThreadArg));
        for (int i = 0; i < THREAD_COUNT; i++) {
                threadArgs[i].threadId = i;
                pthread_create(&threads[i], NULL, threadFunction, (void *) &threadArgs[i]);
        }

        for (int i = 0; i < THREAD_COUNT; i++) {
                pthread_join(threads[i], NULL);
        }

	double curveArea = estimateArea(sorroundingShape, sampleCount);
	std::cout << "The area estimate with " << sampleCount << " samples is: " << curveArea << "\n";

        struct timeval end;
        gettimeofday(&end, NULL);
        double runningTime = ((end.tv_sec + end.tv_usec / 1000000.0)
                        - (start.tv_sec + start.tv_usec / 1000000.0));
        std::cout << "Execution Time: " << runningTime << " Seconds" << std::endl;
	
	return 0;
}
