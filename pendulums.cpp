#include "fpng.h"
#include "doublePendulum.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <thread>
#include <semaphore>
#include <filesystem>

// Program constants
const int IMAGE_WIDTH = 1920;
const int IMAGE_HEIGHT = 1080;
/*
const int IMAGE_WIDTH = 1280;
const int IMAGE_HEIGHT = 720;
*/
/*
const int IMAGE_WIDTH = 852;
const int IMAGE_HEIGHT = 480;
*/
const int GRID_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;

const double PI = 3.14159265358979323846;
const double TAU = PI * 2.0;
const int NUM_THREADS = 16;
const int FRAMERATE = 30;
const int SECONDS = 30;
const double SIM_TIME_PER_FRAME = 0.005;
const double SIM_TIMESTEP_SIZE = 0.0005;
const char* SAVE_DIR = "rendered";

// Function prototypes
void doWorkOnSet(DoublePendulum* pendulums, int rowOffset, int rowCount, int numTotalSteps, int numMiniSteps, double stepSize);
void distributeRows(int* rowCounts);
double angleFromProportion(double value);
void storePendulumColor(char* loc, PendulumColor color);

// Global semaphores
std::counting_semaphore workSemaphore{NUM_THREADS}, doneSemaphore{0};

/**
 * Main function
 */
int main() {
	std::cout << "Initializing" << std::endl;

	fpng::fpng_init();

	// Allocate for the raw image data
	auto imageBuffer = new char[IMAGE_HEIGHT][IMAGE_WIDTH][3];

	// Allocate for all of the pendulums
	DoublePendulum* pendulums = new DoublePendulum[IMAGE_HEIGHT * IMAGE_WIDTH];

	// Allocate threads
	std::thread threads[NUM_THREADS];
	int rowCounts[NUM_THREADS];

	// Runtime values
	int numMiniSteps = SIM_TIME_PER_FRAME / SIM_TIMESTEP_SIZE;
	int totalNumSteps = FRAMERATE * SECONDS;

	distributeRows(rowCounts);

	int rowOffset = 0;

	for (int j = 0; j < IMAGE_HEIGHT; j++) {
		for (int k = 0; k < IMAGE_WIDTH; k++) {
			pendulums[j * IMAGE_WIDTH + k] = DoublePendulum(angleFromProportion((double) j / (double) IMAGE_HEIGHT), angleFromProportion((double) k / (double) IMAGE_WIDTH));
		}
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		threads[i] = std::thread(doWorkOnSet, pendulums, rowOffset, rowCounts[i], totalNumSteps, numMiniSteps, SIM_TIMESTEP_SIZE);
		rowOffset += rowCounts[i];
	}

	// Create the save directory
	std::filesystem::create_directory(SAVE_DIR);

	std::cout << "Simulation parameters:" << std::endl;
	std::cout << "\tTotal number of frames: " << (totalNumSteps * numMiniSteps) << std::endl;
	std::cout << "\tTotal number of pixels per frame: " << (IMAGE_HEIGHT * IMAGE_WIDTH) << std::endl << std::endl;

	std::cout << "Starting simulation" << std::endl;

	int imageCount = 0;
	char filename[255];

	for (int step = 0; step < totalNumSteps; step++) {
		// Here we wait for all of the threads to be done by acquiring the done sempaphore
		for (int i = 0; i < NUM_THREADS; i++) {
			doneSemaphore.acquire();
		}

		// Now we copy all of the pendulums' color values to the image buffer
		for (int j = 0; j < IMAGE_HEIGHT; j++) {
			for (int k = 0; k < IMAGE_WIDTH; k++) {
				storePendulumColor(imageBuffer[j][k], pendulums[j * IMAGE_WIDTH + k].getColor());
			}
		}

		// Now we can let the threads do more work again
		for (int i = 0; i < NUM_THREADS; i++) {
			workSemaphore.release();
		}

		// Now we output the rendered frame
		std::cout << "Image: " << imageCount << std::endl;

		snprintf(filename, 254, "%s/%d.png", SAVE_DIR, imageCount);
		fpng::fpng_encode_image_to_file(filename, (void*)(imageBuffer), IMAGE_WIDTH, IMAGE_HEIGHT, 3);

		imageCount++;
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		threads[i].join();
	}

	return 0;
}

/**
 * This function is what each thread is running when spawned.
 *
 * It runs through each total step of the program, and waits for the main thread to tell it to work by acquiring the work semaphore.
 * Then it does the specified number of simulation steps on each pendulum in the thread's set, and then signals the main thread that it is done.
 */
void doWorkOnSet(DoublePendulum* pendulums, int rowOffset, int rowCount, int numTotalSteps, int numMiniSteps, double stepSize) {

	for (int step = 0; step < numTotalSteps; step++) {
		workSemaphore.acquire();

		for (int i = 0; i < numMiniSteps; i++) {
			for (int j = rowOffset; j < rowOffset + rowCount; j++) {
				for (int k = 0; k < IMAGE_WIDTH; k++) {
					pendulums[j * IMAGE_WIDTH + k].step(stepSize);
				}
			}
		}

		doneSemaphore.release();
	}

}

/**
 * This function distributes the rows of the pendulum array to each thread, and
 * inserts the corresponding row counts to the rowCounts array. The last thread in
 * the list takes up any remainder related to dividing the number of rows by the number
 * of available threads.
 */
void distributeRows(int* rowCounts) {
	int numPerThread = IMAGE_HEIGHT / NUM_THREADS;
	int remainder = IMAGE_HEIGHT % NUM_THREADS;

	for (int i = 0; i < NUM_THREADS; i++) {
		rowCounts[i] = numPerThread;
	}

	rowCounts[NUM_THREADS - 1] += remainder;
}

/**
 * Helpful function that stores the PendulumColor from a particular pendulum in a specific
 * location in the imageBuffer.
 */
void storePendulumColor(char* loc, PendulumColor color) {
	loc[0] = color.r;
	loc[1] = color.g;
	loc[2] = color.b;
}

/**
 * The function used to distribute initial pendulum angles based on the porportion of the image dimensions it is located in.
 */
double angleFromProportion(double value) {
	return TAU * value - PI;
}
