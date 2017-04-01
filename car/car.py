import fcntl
import termios
import time
import RPi.GPIO as GPIO

class Car:
    motor1a = 16
    motor1b = 21
    motor2a = 6
    motor2b = 19

    def __init__(self):
        GPIO.setup(self.motor1a,GPIO.OUT)
        GPIO.setup(self.motor1b,GPIO.OUT)
        GPIO.setup(self.motor2a,GPIO.OUT)
        GPIO.setup(self.motor2b,GPIO.OUT)

        print "Ready To Go!"

    def _getch(self):
        
        fd = sys.stdin.fileno()

        oldterm = termios.tcgetattr(fd)
        newattr = termios.tcgetattr(fd)
        newattr[3] = newattr[3] & ~ termios.ICANON & ~ termios.ECHO
        termios.tcsetattr(fd, termios.TCSANOW, newattr)

        oldflags = fcntl.fcntl(fd, fcntl.F_GETFL)
        fcntl.fcntl(fd, fcntl.F_SETFL, oldflags | os.O_NONBLOCK)
        c = ""
        try:
            curr = time.time()
            target = curr + .1
            while curr < target:
                try:
                    c = sys.stdin.read(3)
                    print "pressed ", repr(c)
                except IOError: pass
                curr = time.time()
        finally:
            termios.tcsetattr(fd, termios.TCSAFLUSH, oldterm)
            fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)
            return c;

    def go_forward(self):
        #print "going forward"
        GPIO.output(self.motor1a,GPIO.LOW)
        GPIO.output(self.motor1b,GPIO.HIGH)
        GPIO.output(self.motor2a,GPIO.LOW)
        GPIO.output(self.motor2b,GPIO.HIGH)

    def go_backward(self):
        #print "going backward"
        GPIO.output(self.motor1a,GPIO.HIGH)
        GPIO.output(self.motor1b,GPIO.LOW)
        GPIO.output(self.motor2a,GPIO.HIGH)
        GPIO.output(self.motor2b,GPIO.LOW)		

    def go_left(self):
        #print "going left"
        GPIO.output(self.motor1a,GPIO.HIGH)
        GPIO.output(self.motor1b,GPIO.LOW)
        GPIO.output(self.motor2a,GPIO.LOW)
        GPIO.output(self.motor2b,GPIO.HIGH)

    def go_right(self):
        #print "going right"
        GPIO.output(self.motor1a,GPIO.LOW)
        GPIO.output(self.motor1b,GPIO.HIGH)
        GPIO.output(self.motor2a,GPIO.HIGH)
        GPIO.output(self.motor2b,GPIO.LOW)

    def stop_all(self):
        GPIO.output(self.motor1a,GPIO.LOW)
        GPIO.output(self.motor1b,GPIO.LOW)
        GPIO.output(self.motor2a,GPIO.LOW)
        GPIO.output(self.motor2b,GPIO.LOW)


    def clean(self):
        self.stop_all()
        GPIO.cleanup()

if __name__ == "__main__":
	car = Car()

	command = ""
	quit = False
	manual_toggle = True

	while quit != True:
		command = car.getch()
		if command == "m":
			if manual_toggle:
				manual_toggle = False
				print "Manual Mode is OFF"
			else:
				manual_toggle = True
				print "Manual Mode is ON"

		if manual_toggle:
			if command == "w":
				print "going forward"
				curr = time.time()
				target = curr + .1
				while curr < target:
					car.go_forward()
					curr = time.time()
				command = ""
			elif command == "s":
				print "going backward"
				curr = time.time()
				target = curr + .1
				while curr < target:
					car.go_backward()
					curr = time.time()
				command = ""
			elif command == "a":
				print "going left"
				curr = time.time()
				target = curr + .05
				while curr < target:
					car.go_left()
					curr = time.time()
				command = ""
			elif command == "d":
				print "going right"
				curr = time.time()
				target = curr + .05
				while curr < target:
					car.go_right()
					curr = time.time()
				command = ""
			elif command == "x":
				car.stop_all()
			elif command == "q":
				quit = True
			else:
				command = ""
				car.stop_all()
		else:
			limit = 10
			if (server.car_center_x => server.tar_center_x + limit or server.car_center_x <= server.tar_center_x - limit) and (server.car_center_y => server.tar_center_y + limit or server.car_center_y <= server.tar_center_y - limit):
				print "Inside Target!!!"
				quit = True
			else:
				if server.car_center_x < server.tar_center_x:
					print "going east"
					curr = time.time()
					target = curr + .1
					while curr < target:
						car.go_forward()
						curr = time.time()
					command = ""
				elif server.car_center_x > server.tar_center_x:
					print "going west"
					curr = time.time()
					target = curr + .1
					while curr < target:
						car.go_backward()
						curr = time.time()
					command = ""
				elif server.car_center_y > server.tar_center_y:
					print "going North"
					curr = time.time()
					target = curr + .1
					while curr < target:
						car.go_backward()
						curr = time.time()
					command = ""
				elif server.car_center_y < server.tar_center_y:
					print "going South"
					curr = time.time()
					target = curr + .1
					while curr < target:
						car.go_backward()
						curr = time.time()
					command = ""
				else:
					print "Stopped"
					car.stop_all()



	car.clean()

	print "DONE"