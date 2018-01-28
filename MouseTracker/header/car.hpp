/*
 * car.hpp
 *
 *  Created on: 27 Jan 2018
 *      Author: alan
 */

#ifndef CAR_HPP_
#define CAR_HPP_

#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class Car
{
public:
	Car();
	Car(Point front, Point back, Point centre);
	~Car();
	void set_front(Point p);
	void set_back(Point p);
	void set_centre(Point p);
	Point get_front();
	Point get_back();
	Point get_centre();

private:
	Point front;
	Point back;
	Point centre;
};



#endif /* CAR_HPP_ */
