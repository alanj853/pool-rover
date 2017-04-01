import urllib
import numpy as np
import cv2
import sys

sys.path.append("..\UDP")

from client import UDP_Client

def click_and_crop(event, x, y, flags, param):
    # grab references to the global variables


    # if the left mouse button was clicked, record the starting
    # (x, y) coordinates and indicate that cropping is being
    # performed
    if event == cv2.EVENT_LBUTTONDOWN:
        Camera.refPt = [(x, y)]

    # check to see if the left mouse button was released
    elif event == cv2.EVENT_LBUTTONUP:
        Camera.refPt.append((x,y))

    # try:

    #     print "A ", Camera.refPt[0]
    # except IndexError, NameError:
    #     print "X: ", x, " Y:", y

    # try:
    #     print "B ", Camera.refPt[1]
    # except IndexError:
    #     print "X: ", x, " Y:", y
    
    # print ""

class Camera:
    
    refPt = []

    def __init__(self, camera, ip):
        self.client = UDP_Client(ip, 5005)
        self.cap = cv2.VideoCapture(camera)
        #self.refPt = []
        self.run_camera = True
        self.f_x1 = 0
        self.f_y1 = 0
        self.f_x2 = 0
        self.f_y2 = 0
        self.t_x1 = 0
        self.t_y1 = 0
        self.t_x2 = 0
        self.t_y2 = 0
        self.c_x1 = 0
        self.c_y1 = 0
        self.c_x2 = 0
        self.c_y2 = 0

    def turn_off(self):
        self.run_camera = False

    def get_car_coord(self):
        arr = []
        arr.append(self.c_x1)
        arr.append(self.c_y1)
        arr.append(self.c_x2)
        arr.append(self.c_y2)

        data = "C:" + str(self.c_x1) + ":"+ str(self.c_y1) + ":"+ str(self.c_x2) + ":"+ str(self.c_y2) + ":E"
        print "Car: (", self.c_x1, " ", self.c_y1, ") (", self.c_x2, " ", self.c_y2, ") "
        return data

    def get_target_coord(self):
        arr = []
        arr.append(self.t_x1)
        arr.append(self.t_y1)
        arr.append(self.t_x2)
        arr.append(self.t_y2)

        data = "T:" + str(self.t_x1) + ":"+ str(self.t_y1) + ":"+ str(self.t_x2) + ":"+ str(self.t_y2) + ":E"
        print "Target: (", self.t_x1, " ", self.t_y1, ") (", self.t_x2, " ", self.t_y2, ") "
        return data

    def get_frame_coord(self):
        arr = []
        arr.append(self.f_x1)
        arr.append(self.f_y1)
        arr.append(self.f_x2)
        arr.append(self.f_y2)

        data = "F:" + str(self.f_x1) + ":"+ str(self.f_y1) + ":"+ str(self.f_x2) + ":"+ str(self.f_y2) + ":E"
        print "Frame: (", self.f_x1, " ", self.f_y1, ") (", self.f_x2, " ", self.f_y2, ") "
        return data

    def draw_rectangles(self):
        cv2.rectangle(self.frame, (self.c_x1, self.c_y1), (self.c_x2, self.c_y2), (255, 255, 0), 2)
        cv2.rectangle(self.frame, (self.t_x1, self.t_y1), (self.t_x2, self.t_y2), (0, 255, 255), 2)
        cv2.rectangle(self.frame, (self.f_x1, self.f_y1), (self.f_x2, self.f_y2), (255, 255, 255), 2)
        cv2.imshow("frame", self.frame)
        

    def run(self):
        count = 0
        a = (0,0)
        b = (0,0)

        car_assigned = False
        target_assigned = False
        frame_assigned = False

        while(self.run_camera):
            
            # Capture frame-by-frame
            ret, self.frame = self.cap.read()

            cv2.imshow('frame',self.frame)
            if count < 1:
                cv2.setMouseCallback('frame', click_and_crop)
            if count > 100:
                count = 0
            count = count + 1

            cv2.rectangle(self.frame, a, b, (0, 255, 0), 2)
            

            if frame_assigned:
                cv2.rectangle(self.frame, (self.f_x1, self.f_y1), (self.f_x2, self.f_y2), (255, 255, 255), 2)

            if target_assigned:
                cv2.rectangle(self.frame, (self.t_x1, self.t_y1), (self.t_x2, self.t_y2), (0, 255, 255), 2)
            
            if car_assigned:
                cv2.rectangle(self.frame, (self.c_x1, self.c_y1), (self.c_x2, self.c_y2), (255, 255, 0), 2)

            cv2.imshow("frame", self.frame)

            try:
                a = self.refPt[0]
                b = self.refPt[1]
            except IndexError:
                a = (0,0)
                b = (0,0)

            input = cv2.waitKey(1) & 0xFF 
            if input == ord('q'):
                self.client.send("Q:E")
                break
            elif input == ord('f'):
                print "Frame Assigned ", a, " ", b
                self.f_x1 = a[0]
                self.f_y1 = a[1]
                self.f_x2 = b[0]
                self.f_y2 = b[1]
                frame_assigned = True
            elif input == ord('t'):
                print "Target Assigned ", a, " ", b
                self.t_x1 = a[0]
                self.t_y1 = a[1]
                self.t_x2 = b[0]
                self.t_y2 = b[1]
                target_assigned = True
            elif input == ord('c'):
                print "Car Assigned ", a, " ", b
                self.c_x1 = a[0]
                self.c_y1 = a[1]
                self.c_x2 = b[0]
                self.c_y2 = b[1]
                car_assigned = True
            elif input == ord('s'):
                print "Sending Coordinates to Car ..."
                car_data = self.get_car_coord()
                target_data = self.get_target_coord()
                frame_data = self.get_frame_coord()
                self.client.send(car_data)
                self.client.send(frame_data)
                self.client.send(target_data)
                self.client.send("P:E")
            elif input == ord('d'):
                print "Drawing rectangles"
                cv2.rectangle(self.frame, (self.c_x1, self.c_y1), (self.c_x2, self.c_y2), (255, 255, 0), 2)
                cv2.rectangle(self.frame, (self.t_x1, self.t_y1), (self.t_x2, self.t_y2), (0, 255, 255), 2)
                cv2.rectangle(self.frame, (self.f_x1, self.f_y1), (self.f_x2, self.f_y2), (255, 255, 255), 2)
                cv2.imshow("frame", self.frame)
                cv2.waitKey(1000)
            elif input == ord('r'):
                print "All Assignments Reset "
                self.f_x1 = 0
                self.f_y1 = 0
                self.f_x2 = 0
                self.f_y2 = 0
                self.t_x1 = 0
                self.t_y1 = 0
                self.t_x2 = 0
                self.t_y2 = 0
                self.c_x1 = 0
                self.c_y1 = 0
                self.c_x2 = 0
                self.c_y2 = 0
                frame_assigned = False
                target_assigned = False
                car_assigned = False



        # When everything done, release the capture
        self.cap.release()
        cv2.destroyAllWindows()

if __name__ == '__main__':
    camera = 0
    ip = "127.0.0.1"

    
    try:
        ip = sys.argv[1]
    except IndexError:
        print "No Camera Given: using 0"

    try:
        ip = sys.argv[2]
    except IndexError:
        print "No IP Given: using 127.0.0.1"

    cam = Camera(camera, ip)
    cam.run()