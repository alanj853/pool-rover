import urllib
import numpy as np
import cv2
 
cap = cv2.VideoCapture(0)
refPt = []
ready_to_draw = False


def click_and_crop(event, x, y, flags, param):
    	# grab references to the global variables
    
    cropping = False
    global refPt, frame
 
	# if the left mouse button was clicked, record the starting
	# (x, y) coordinates and indicate that cropping is being
	# performed
    if event == cv2.EVENT_LBUTTONDOWN:
        refPt = [(x, y)]
        cropping = True
 
    # check to see if the left mouse button was released
    elif event == cv2.EVENT_LBUTTONUP:
        refPt.append((x,y))
        cropping = False
        ready_to_draw = True


    try:

        print "A ", refPt[0]
    except IndexError:
        print "X: ", x, " Y:", y

    try:
        print "B ", refPt[1]
    except IndexError:
        print "X: ", x, " Y:", y
    
    print ""


count = 0
a = (0,0)
b = (0,0)

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    #gray = cv2.cvtColor(frame, 1)

    # Display the resulting frame
    
    #cv2.rectangle(frame, (106, 44), (313, 207), (0, 255, 255), 2)
    
    cv2.imshow('frame',frame)
    if count < 1:
        cv2.setMouseCallback('frame', click_and_crop)
    if count > 100:
        count = 0
    count = count + 1

    cv2.rectangle(frame, a, b, (0, 255, 0), 2)
    cv2.imshow("frame", frame)

    try:
        a = refPt[0]
        b = refPt[1]
    except IndexError:
        a = (0,0)
        b = (0,0)

    # if ready_to_draw:
    #     try:
    #         cv2.rectangle(frame, refPt[0], refPt[1], (0, 255, 255), 2)
    #     except IndexError as e:
    #         print "Error: ", e
        
    

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

#cv2.rectangle(frame, refPt[0], refPt[1], (0, 255, 0), 2)
#cv2.imshow("frame", frame)
#cv2.waitKey(0)

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()