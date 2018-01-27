/*
 This is the main image processing program. It can be run with or without
 an argument. The argument that can be passed in is the camera device number
 the user wished to use (usually within 0-10)

 In order to run this program, opencv will need to be installed, the libraries
 will have to be added to the program and the include directories as well.
 */

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>

#include <error.h>

using namespace std;
using namespace cv;

#define BLUE Scalar(255, 0, 0)
#define GREEN Scalar(0, 255, 0)
#define RED Scalar(0, 0, 255)

// Headers
void pause();
void CallBackFunc(int event, int x, int y, int flags, void* userdata);
string intToString(int x);
void draw_circle(int x, int y, string message);
void draw_circle(int x, int y, string message, Scalar colour);

Mat im_rgb;
int mouse_coordinates_x = 90;
int mouse_coordinates_y = 90;
int car_coordinates_x = 0;
int car_coordinates_y = 0;
int dest_coordinates_x = 0;
int dest_coordinates_y = 0;
VideoCapture camera;

// function to try to open a camera. If the camera name 
// specified in the argument cannot be opened, the function
// tries to open the first 5 default cameras connected to the 
// machine until one is sucessfully connected.
bool determine_camera(char *camera_name) {
	if (camera.open(camera_name)) {
		cout << "Using Camera: " << camera_name << endl;
		return true;
	} else {
		cout << "Camera '" << camera_name << "' is not available" << endl;
		int i = 0;
		while (i < 5) {
			if (camera.open(i)) {
				cout << "Using Camera: " << i << endl;
				return true;
			}
			i++;
		}
		cout << "No Camera Available" << endl;
		return false;
	}
}

// method to run  entire image processing program
int run_all(char *cameraName) {

	string mouse_coordinates_x_s = "";
	string mouse_coordinates_y_s = "";

	if (!determine_camera(cameraName)) // if camera cannot be found exit program
		return -CAMERA_NOT_FOUND;

	while (1) {

		camera.read(im_rgb); // read image frame

		namedWindow("Original", WINDOW_NORMAL);
		setMouseCallback("Original", CallBackFunc, NULL);
		draw_circle(mouse_coordinates_x, mouse_coordinates_y, "Mouse");
		if (car_coordinates_x != 0 || car_coordinates_y != 0)
			draw_circle(car_coordinates_x, car_coordinates_y, "Car", GREEN);
		if (dest_coordinates_x != 0 || dest_coordinates_y != 0)
			draw_circle(dest_coordinates_x, dest_coordinates_y, "Destination",
					BLUE);
		imshow("Original", im_rgb);

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		// press 'd' to exit the program
		if ((char) waitKey(30) == 'd')
			break;

		// Clear Car and Destination Coordinates
		if ((char) waitKey(30) == 'c') {
			dest_coordinates_x = 0;
			dest_coordinates_y = 0;
			car_coordinates_x = 0;
			car_coordinates_y = 0;
		}
	}

	camera.release();
	return NORMAL_EXIT;
}

int main(int argc, char *argv[]) {
	int result;
	if (argc < 2) {
		char x = '1';
		char *y = &x;
		result = run_all(y);
	} else {
		result = run_all(argv[1]);
	}

	cout << "Result of program: " << result << endl;
	//	pause();
	return result;
}

void pause() {
	cout << "Press Enter to Continue..." << endl;
	cin.get();
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		dest_coordinates_x = x;
		dest_coordinates_y = y;
		cout << "Left button of the mouse is clicked - position (" << x << ", "
				<< y << ")" << endl;
	} else if (event == EVENT_RBUTTONDOWN) {
		car_coordinates_x = x;
		car_coordinates_y = y;
		cout << "Right button of the mouse is clicked - position (" << x << ", "
				<< y << ")" << endl;
	} else if (event == EVENT_MBUTTONDOWN) {
		cout << "Middle button of the mouse is clicked - position (" << x
				<< ", " << y << ")" << endl;
	} else if (event == EVENT_MOUSEMOVE) {
		cout << "Mouse move over the window - position (" << x << ", " << y
				<< ")" << endl;

	}

	mouse_coordinates_x = x;
	mouse_coordinates_y = y;
}

string intToString(int x) {

	std::stringstream ss;
	ss << x;
	return ss.str();
}

void draw_circle(int x, int y, string message) {
	draw_circle(x, y, message, RED);
}

void draw_circle(int x, int y, string message, Scalar colour) {
	circle(im_rgb, Point(x, y), 10, colour);
	string x_s = intToString(x);
	string y_s = intToString(y);
	putText(im_rgb, message + " (" + x_s + "," + y_s + ")", Point(x, (y - 30)),
			1, 2, colour);
}
