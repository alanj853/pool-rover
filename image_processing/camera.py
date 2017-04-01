import urllib
import numpy as np
import cv2

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

    def __init__(self, camera):
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

        print "Car: (", self.c_x1, " ", self.c_y1, ") (", self.c_x2, " ", self.c_y2, ") "
        return arr

    def get_target_coord(self):
        arr = []
        arr.append(self.t_x1)
        arr.append(self.t_y1)
        arr.append(self.t_x2)
        arr.append(self.t_y2)

        print "Target: (", self.t_x1, " ", self.t_y1, ") (", self.t_x2, " ", self.t_y2, ") "
        return arr

    def get_frame_coord(self):
        arr = []
        arr.append(self.f_x1)
        arr.append(self.f_y1)
        arr.append(self.f_x2)
        arr.append(self.f_y2)

        print "Frame: (", self.f_x1, " ", self.f_y1, ") (", self.f_x2, " ", self.f_y2, ") "
        return arr

    def run(self):
        count = 0
        a = (0,0)
        b = (0,0)

        while(self.run_camera):
            
            # Capture frame-by-frame
            ret, frame = self.cap.read()

            cv2.imshow('frame',frame)
            if count < 1:
                cv2.setMouseCallback('frame', click_and_crop)
            if count > 100:
                count = 0
            count = count + 1

            cv2.rectangle(frame, a, b, (0, 255, 0), 2)
            cv2.imshow("frame", frame)

            try:
                a = self.refPt[0]
                b = self.refPt[1]
            except IndexError:
                a = (0,0)
                b = (0,0)

            input = cv2.waitKey(1) & 0xFF 
            if input == ord('q'):
                break
            elif input == ord('f'):
                print "Frame Assigned ", a, " ", b
                self.f_x1 = a[0]
                self.f_y1 = a[1]
                self.f_x2 = b[0]
                self.f_y2 = b[1]
            elif input == ord('t'):
                print "Target Assigned ", a, " ", b
                self.t_x1 = a[0]
                self.t_y1 = a[1]
                self.t_x2 = b[0]
                self.t_y2 = b[1]
            elif input == ord('c'):
                print "Car Assigned ", a, " ", b
                self.c_x1 = a[0]
                self.c_y1 = a[1]
                self.c_x2 = b[0]
                self.c_y2 = b[1]
            elif input == ord('s'):
                print "Sending Coordinates to Car ..."
                self.get_car_coord()
                self.get_frame_coord()
                self.get_target_coord()
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



        # When everything done, release the capture
        self.cap.release()
        cv2.destroyAllWindows()

if __name__ == '__main__':
    cam = Camera(0)
    cam.run()