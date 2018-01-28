/*
 * car.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: alan
 */

#include <car.hpp>

#include <opencv2/opencv.hpp>



Car::Car() {
}

Car::Car(Point front, Point back, Point centre) {
	this->front = front;
	this->back = back;
	this->centre = centre;
}

Car::~Car() {
}

void Car::set_front(Point p) {
	front = p;
}

void Car::set_back(Point p){
	back = p;
}

void Car::set_centre(Point p){
	centre = p;
}

Point Car::get_front(){
	return front;
}

Point Car::get_back(){
	return back;
}

Point Car::get_centre(){
	return centre;
}




