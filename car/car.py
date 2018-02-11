import fcntl
import termios
import time
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)
import math
import sys
import os
import logging
import socket
import struct


class Car:
	enable1 = 7
	enable2 = 12
	motor1a = 3
	motor1b = 5
	motor2a = 8
	motor2b = 10

	def __init__(self):
		GPIO.setup(self.motor1a,GPIO.OUT)
		GPIO.setup(self.motor1b,GPIO.OUT)
		GPIO.setup(self.motor2a,GPIO.OUT)
		GPIO.setup(self.motor2b,GPIO.OUT)
		GPIO.setup(self.enable1,GPIO.OUT)
		GPIO.setup(self.enable2,GPIO.OUT)

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
		# print "going forward"
		GPIO.output(self.enable1,GPIO.HIGH)
		GPIO.output(self.enable2,GPIO.HIGH)
		GPIO.output(self.motor1a,GPIO.LOW)
		GPIO.output(self.motor1b,GPIO.HIGH)
		GPIO.output(self.motor2a,GPIO.HIGH)
		GPIO.output(self.motor2b,GPIO.LOW)

	def go_backward(self):
		# print "going backward"
		GPIO.output(self.enable1,GPIO.HIGH)
		GPIO.output(self.enable2,GPIO.HIGH)
		GPIO.output(self.motor1a,GPIO.HIGH)
		GPIO.output(self.motor1b,GPIO.LOW)
		GPIO.output(self.motor2a,GPIO.LOW)
		GPIO.output(self.motor2b,GPIO.HIGH)		

	def go_left(self):
		# print "going left"
		GPIO.output(self.enable1,GPIO.HIGH)
		GPIO.output(self.enable2,GPIO.HIGH)
		GPIO.output(self.motor1a,GPIO.LOW)
		GPIO.output(self.motor1b,GPIO.HIGH)
		GPIO.output(self.motor2a,GPIO.LOW)
		GPIO.output(self.motor2b,GPIO.HIGH)	

	def go_right(self):
		# print "going right"
		GPIO.output(self.enable1,GPIO.HIGH)
		GPIO.output(self.enable2,GPIO.HIGH)
		GPIO.output(self.motor1a,GPIO.HIGH)
		GPIO.output(self.motor1b,GPIO.LOW)
		GPIO.output(self.motor2a,GPIO.HIGH)
		GPIO.output(self.motor2b,GPIO.LOW)	

	def stop_all(self):
		GPIO.output(self.motor1a,GPIO.LOW)
		GPIO.output(self.motor1b,GPIO.LOW)
		GPIO.output(self.motor2a,GPIO.LOW)
		GPIO.output(self.motor2b,GPIO.LOW)
		GPIO.output(self.enable1,GPIO.LOW)
		GPIO.output(self.enable2,GPIO.LOW)


	def clean(self):
		self.stop_all()
		GPIO.cleanup()

if __name__ == "__main__":
	car = Car()
	host = '192.168.1.9'
	port = 6000
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	timeval = struct.pack('ll', 10, 0)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_SNDTIMEO, timeval)

	print "Listening on udp %s:%s" % (host, port)
	s.bind((host, port))

	command = ""
	quit = False
	manual_toggle = True

	while quit != True:
		command = car._getch()
		if command == "m":
			if manual_toggle:
				manual_toggle = False
				print "Manual Mode is OFF"
			else:
				manual_toggle = True
				print "Manual Mode is ON"
		
		if command == "q":
			quit = True

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
			# elif command == "q":
			# 	quit = True
			else:
				command = ""
				car.stop_all()
		else:
			(data, addr) = s.recvfrom(128*1024)
			print "From PC: ", data

			if data == "Rotate":
				curr = time.time()
				target = curr + .05
				while curr < target:
					car.go_right()
					curr = time.time()
				command = ""
			elif data == "Rotate Slowly":
				curr = time.time()
				target = curr + .05
				while curr < target:
					car.go_right()
					curr = time.time()
				command = ""
			elif data == "Move Forward":
				curr = time.time()
				target = curr + .1
				while curr < target:
					car.go_forward()
					curr = time.time()
				command = ""
			elif data == "TURN OFF":
				quit = True
			else:
				command = ""
				car.stop_all()





	car.clean()
	s.close()

	print "DONE"
