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
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <cmath>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>

#include <error.hpp>
#include <car.hpp>
#include <utils.hpp>
#include <ObjectFinder.hpp>
#include <udp_client.hpp>
 
using namespace cv;
using namespace std;
 
// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

using namespace std;
using namespace cv;
using namespace udp_client_server;

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
int track();

int comp = 1;
int useSavedObject = 0;
int detectObject = 0;
const string trackbarWindowName = "Trackbars";
int MIN = 0;
int MAX = 255;
//VideoCapture camera;
ObjectFinder finder;
// udp_client client;

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

const int UDP_PORT=6000;
const unsigned int UDP_MSG_SIZE=20;
udp_client client("127.0.0.1", UDP_PORT);

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

	//return camera.open("http://192.168.1.18:8080/video?x.mjpeg");
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

	// udp_client client("127.0.0.1", 6000);

	// List of tracker types in OpenCV 3.2
    // NOTE : GOTURN implementation is buggy and does not work.
    string trackerTypes[6] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN"};
    // vector <string> trackerTypes(types, std::end(types));
 
    // Create a tracker
    string trackerType = trackerTypes[2];
 
    Ptr<Tracker> tracker1;
	Ptr<Tracker> tracker2;
	Ptr<Tracker> tracker3;


	string mouse_coordinates_x_s = "";
	string mouse_coordinates_y_s = "";

	#if (CV_MINOR_VERSION < 3)
    {
        tracker1 = Tracker::create(trackerType);
		tracker2 = Tracker::create(trackerType);
		tracker3 = Tracker::create(trackerType);
    }
    #else
    {
        if (trackerType == "BOOSTING")
            tracker1 = TrackerBoosting::create();
			tracker2 = TrackerBoosting::create();
			tracker3 = TrackerBoosting::create();
        if (trackerType == "MIL")
            tracker1 = TrackerMIL::create();
			tracker2 = TrackerBoosting::create();
			tracker3 = TrackerBoosting::create();
        if (trackerType == "KCF")
            tracker1 = TrackerKCF::create();
			tracker2 = TrackerBoosting::create();
			tracker3 = TrackerBoosting::create();
        if (trackerType == "TLD")
            tracker1 = TrackerTLD::create();
			tracker2 = TrackerBoosting::create();
			tracker3 = TrackerBoosting::create();
        if (trackerType == "MEDIANFLOW")
            tracker1 = TrackerMedianFlow::create();
			tracker2 = TrackerBoosting::create();
			tracker3 = TrackerBoosting::create();
        if (trackerType == "GOTURN")
            tracker1 = TrackerGOTURN::create();
			tracker2 = TrackerBoosting::create();
			tracker3 = TrackerBoosting::create();
    }
    #endif

	if (!determine_camera(cameraName)) // if camera cannot be found exit program
		return -CAMERA_NOT_FOUND;

	camera.read(im_rgb);
     
    // Define initial boundibg box
    Rect2d bbox_car(287, 23, 86, 320);
	Rect2d bbox_front(287, 23, 86, 320);
	Rect2d bbox_back(287, 23, 86, 320);

	double start = time(0);
	double end = start + 5; // allow 10 sec to warm up

	cout << "Waiting..." << endl;
	while (time(0) < end)
	{
		camera.read(im_rgb);
	}
    cout << "Select Entire Car" << endl;
	bbox_car = selectROI(im_rgb, false);
	cout << "Select Front of Car" << endl;
	bbox_front = selectROI(im_rgb, false);
	cout << "Select Back of Car" << endl;
	bbox_back = selectROI(im_rgb, false);

	tracker1->init(im_rgb, bbox_car);
	tracker2->init(im_rgb, bbox_front);
	tracker3->init(im_rgb, bbox_back);

	while (1) {

		camera.read(im_rgb); // read image frame

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

		// Start timer
        double timer = (double)getTickCount();
         
        // Update the tracking result
        bool ok1 = tracker1->update(im_rgb, bbox_car);
		bool ok2 = tracker2->update(im_rgb, bbox_front);
		bool ok3 = tracker3->update(im_rgb, bbox_back);

		car_coordinates_b_x = ((bbox_back.br() + bbox_back.tl())*0.5).x; //bbox_back.center.x;
		car_coordinates_b_y = ((bbox_back.br() + bbox_back.tl())*0.5).y;

		car_coordinates_f_x = ((bbox_front.br() + bbox_front.tl())*0.5).x;
		car_coordinates_f_y = ((bbox_front.br() + bbox_front.tl())*0.5).y;


		if(car_coordinates_b_y < car_coordinates_f_y){
			cout << "Facing South" << endl;
		}
		else if(car_coordinates_b_y > car_coordinates_f_y){
			cout << "Facing North" << endl;
		}
		else {
			cout << "Neither North or South." << endl;
		}


		if(car_coordinates_b_x < car_coordinates_f_x){
			cout << "	East" << endl;
		}
		else if(car_coordinates_b_x > car_coordinates_f_x){
			cout << "	West" << endl;
		}
		else {
			cout << "Neither West or East :(" << endl;
		}

         
        // Calculate Frames per second (FPS)
        float fps = getTickFrequency() / ((double)getTickCount() - timer);
         
        if (ok1 && ok2 && ok3)
        {
            // Tracking success : Draw the tracked object
            rectangle(im_rgb, bbox_car, Scalar( 255, 0, 0 ), 2, 1 );
			rectangle(im_rgb, bbox_front, Scalar( 255, 0, 0 ), 2, 1 );
			rectangle(im_rgb, bbox_back, Scalar( 255, 0, 0 ), 2, 1 );
        }
        else
        {
            // Tracking failure detected.
            putText(im_rgb, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
        }
         
        // Display tracker type on frame
        putText(im_rgb, trackerType + " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50),2);
         
        // Display FPS on frame
        putText(im_rgb, "FPS : " + SSTR(int(fps)), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);


		imshow("Original", im_rgb);

		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		// press 'q' to exit the program
		if ((char) waitKey(30) == 'q')
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

	// int result = track();

	cout << "Result of program: " << result << endl;
	//	pause();
	return result;
}

void pause() {
	cout << "Press Enter to Continue..." << endl;
	cin.get();
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	Mat frame;
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
	char *cstr = new char[message.length() + 1];
	strcpy(cstr, message.c_str());
	client.send(cstr, message.length());
	cout << "Sent: " << message << endl;
}

