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
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>

#include <error.h>
#include <car.hpp>
#include <ObjectFinder.hpp>

using namespace std;
using namespace cv;

/*
 * Colours
 */
#define BLUE Scalar(255, 0, 0)
#define GREEN Scalar(0, 255, 0)
#define RED Scalar(0, 0, 255)
#define YELLOW Scalar(0, 255, 255)

// Headers
void pause();
void CallBackFunc(int event, int x, int y, int flags, void* userdata);
string intToString(int x);
void draw_circle(int x, int y, string message);
void draw_circle(int x, int y, string message, Scalar colour);
void draw_line(int x1, int y1, int x2, int y2, Scalar colour);
Point midpoint(Point a, Point b);
Point midpoint(int x1, int y1, int x2, int y2);
double slope(Point a, Point b);
double slope(int x1, int y1, int x2, int y2);
double length(Point a, Point b);
double length(int x1, int y1, int x2, int y2);
void draw_car();
void adjust_car();
void send_udp_message(string message);
void createTrackbars();

int comp = 1;
int useSavedObject = 0;
int detectObject = 0;
const string trackbarWindowName = "Trackbars";
int MIN = 0;
int MAX = 255;
//VideoCapture camera;
ObjectFinder finder;

//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

string windowName1 = "Transformed Image (Black and White)";
string windowName2 = "Transformed Image (Black and White Inverted)";
string windowName3 = "Transformed Image (HSV)";

void on_trackbar(int, void*) {
	cout << "useSavedObject changed to " << useSavedObject << endl;
	finder.updateMAX_OBJECT_AREA();

}

Mat im_rgb;

int mouse_coordinates_x = 90;
int mouse_coordinates_y = 90;

int car_coordinates_b_x = 0;
int car_coordinates_b_y = 0;
int car_coordinates_c_x = 0;
int car_coordinates_c_y = 0;
int car_coordinates_f_x = 0;
int car_coordinates_f_y = 0;

int car_switch = 0;

const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;


int dest_coordinates_x = 0;
int dest_coordinates_y = 0;
VideoCapture camera;
Car car;
double const slope_tolerance = 0.005;
double const distance_from_target_tolerance = 50;

// function to try to open a camera. If the camera name 
// specified in the argument cannot be opened, the function
// tries to open the first 5 default cameras connected to the 
// machine until one is successfully connected.
bool determine_camera(char *camera_name) {
//	if (camera.open(camera_name)) {
//		cout << "Using Camera: " << camera_name << endl;
//		return true;
//	} else {
//		cout << "Camera '" << camera_name << "' is not available" << endl;
//		int i = 0;
//		while (i < 5) {
//			if (camera.open(i)) {
//				cout << "Using Camera: " << i << endl;
//				return true;
//			}
//			i++;
//		}
//		cout << "No Camera Available" << endl;
//		return false;
//	}

	return camera.open(1);
}

Mat complementImage(Mat im, int rows, int cols) {
	Mat newmat = Mat::zeros(im.rows, im.cols, CV_8UC1);

	int byte = 0;
	int zero = 0;
	int one = 255;

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			byte = im.at<unsigned char>(i, j);
			if (byte != 0)
				newmat.at<unsigned char>(i, j) = zero; // set value at i,j to zero
			else
				newmat.at<unsigned char>(i, j) = one;
		}
	return newmat;
}

void drawObject(int x, int y,Mat &frame){

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

    //UPDATE:JUNE 18TH, 2013
    //added 'if' and 'else' statements to prevent
    //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

}
void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);


	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);



}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed){

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
				//draw object location on screen
				drawObject(x,y,cameraFeed);}

		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

// method to run  entire image processing program
int run_all(char *cameraName) {

	createTrackbars(); //create slider bars for HSV filtering

	string mouse_coordinates_x_s = "";
	string mouse_coordinates_y_s = "";
	bool trackObjects = true;
	bool useMorphOps = false;
	Mat im_gray;   // no need to load the Mat with anything when declaring it.
	Mat img_bw;
	Mat img_bw_comp;
	Mat HSV;
	Mat threshold;
	int x=0, y=0;

	if (!determine_camera(cameraName)) // if camera cannot be found exit program
		return -CAMERA_NOT_FOUND;

	while (1) {

		camera.read(im_rgb); // read image frame

		cvtColor(im_rgb, im_gray, CV_RGB2GRAY); // convert image to grayscale
		cvtColor(im_rgb, HSV, COLOR_BGR2HSV);
		inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX),
				threshold);

		if(useMorphOps)
			morphOps(threshold);
				//pass in thresholded frame to our object tracking function
				//this function will return the x and y coordinates of the
				//filtered object
		if(trackObjects)
			trackFilteredObject(x,y,threshold,im_rgb);

				//show frames
				imshow(windowName2,threshold);
				imshow(windowName3,im_rgb);
				imshow(windowName1,HSV);



		namedWindow("Original", WINDOW_NORMAL);
		setMouseCallback("Original", CallBackFunc, NULL);
		draw_circle(mouse_coordinates_x, mouse_coordinates_y, "Mouse");

		adjust_car();

		if (dest_coordinates_x != 0 || dest_coordinates_y != 0)
			draw_circle(dest_coordinates_x, dest_coordinates_y, "Destination",
			BLUE);
		if (car.get_centre().x != 0 && car.get_centre().y != 0
				&& dest_coordinates_x != 0 && dest_coordinates_y != 0)
			draw_line(car.get_centre().x, car.get_centre().y, dest_coordinates_x,
					dest_coordinates_y, YELLOW);

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
			car_coordinates_c_x = 0;
			car_coordinates_c_y = 0;
			car_coordinates_f_x = 0;
			car_coordinates_f_y = 0;
			car_coordinates_b_x = 0;
			car_coordinates_b_y = 0;
		}
	}
	destroyWindow(windowName1);
	destroyWindow(windowName2);
	destroyWindow(windowName3);

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
		if(car_switch == 0){
			car_coordinates_b_x = x;
			car_coordinates_b_y = y;
			car_switch++;
		}else {
			car_coordinates_f_x = x;
			car_coordinates_f_y = y;
			car_switch = 0;
		}
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

void draw_line(int x1, int y1, int x2, int y2, Scalar colour) {
	double len = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	Point midpoint = Point((x1 + x2) / 2, (y1 + y2) / 2);
	putText(im_rgb, "LEN = " + to_string(len), midpoint, 1, 2, colour);
	line(im_rgb, Point(x1, y1), Point(x2, y2), colour, 1, 4);
	draw_circle((x1 + x2) / 2, (y1 + y2) / 2, "Midpoint", colour);
}

Point midpoint(Point a, Point b) {
	return midpoint(a.x, a.y, b.x, b.y);
}

Point midpoint(int x1, int y1, int x2, int y2) {
	return Point((x1 + x2) / 2, (y1 + y2) / 2);
}

double slope(Point a, Point b) {
	return slope(a.x, a.y, b.x, b.y);
}

double slope(int x1, int y1, int x2, int y2) {
	return (float) (y2 - y1) / (x2 - x1);
}

double length(Point a, Point b) {
	return length(a.x, a.y, b.x, b.y);
}

double length(int x1, int y1, int x2, int y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void draw_car() {
		Point front = Point(car_coordinates_f_x, car_coordinates_f_y);
		Point back = Point(car_coordinates_b_x, car_coordinates_b_y);

		arrowedLine(im_rgb, back, front, GREEN);

		Point centre = midpoint(front, back);


		car.set_centre(centre);
		car.set_front(front);
		car.set_back(back);

		if(car_switch == 0){
			draw_circle(front.x, front.y, "front", GREEN);
			draw_circle(back.x, back.y, "back", GREEN);
			draw_circle(centre.x, centre.y, "Car", GREEN);

		}



}

/*
 * Goal here is to adjust car so that it is facing its destination
 * This means adjusting the slope of the line between the front and
 * the back of the car until it is equal to the slope of the line
 * between the centre of the back and the destination point. Adjustments
 * are done by rotating the car.
 *
 * ============================ SUDO CODE START =================================
 * while len(front_to_dest > len(back_to_des)
 * 	rotate clockwise
 *
 * while (slope(front, back) != (slope(dest, centre) + 10) && slope(front, back) != slope(dest, centre) - 10)
 * 	rotate clockwise
 *
 * we are now facing the right way, and have the correct slope. So just move forward!
 * ============================ SUDO CODE END =================================
 *
 * We don't actually move the car here, just send the data to it.
 */
void adjust_car() {
	if ( (car_coordinates_f_x != 0 || car_coordinates_f_y != 0) && (car_coordinates_b_x != 0 || car_coordinates_b_y != 0) ) {
		draw_car();

		double s1 = slope(car.get_front(), car.get_back());
		double s2 = slope(car.get_centre(), Point(dest_coordinates_x, dest_coordinates_y));

		double slope_diff = sqrt((s1 - s2)*(s1 - s2));

//		cout << "s1 = " << s1 << " s2 = " << s2 << " slope_diff = " << slope_diff << endl;

		if ( length(car.get_front(), Point(dest_coordinates_x, dest_coordinates_y)) > length(car.get_back(), Point(dest_coordinates_x, dest_coordinates_y)) )
			send_udp_message("Rotate");
		else if ( slope_diff > slope_tolerance*10 ) {
			send_udp_message("Rotate Slowly");
		} else if (length(car.get_centre(), Point(dest_coordinates_x, dest_coordinates_y))
				<= distance_from_target_tolerance) {
			send_udp_message("STOP");
		} else {
			send_udp_message("Move Forward");
		}
	}
}

void send_udp_message(string message) {
	cout << message << endl;
}
void createTrackbars(){
	//create window for trackbars


    namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->
    createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
    createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
    createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
    createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
    createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
    createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );


}
