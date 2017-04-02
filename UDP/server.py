import socket
import sys

class UDP_SERVER:

    def __init__(self, ip, port):
        self.PORT = port
        self.IP_ADDR = ip
        self.run_server = True
        self.count = 0
        self.car_center_x = 0
        self.car_center_y = 0
        self.tar_center_x = 0
        self.tar_center_y = 0
        self.east_point = 0
        self.west_point = 0
        self.slope_car = 0
        self.slope_tar = 0
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

        ## Initialise
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        self.sock.bind((self.IP_ADDR, self.PORT))

        print "Waiting for connection..."

    def run(self):
        while self.run_server:
            data, addr = self.sock.recvfrom(1024) # buffer size is 1024 bytes
            print "received message:", data
            
            arr = data.split(":")
            if arr[0] == "C":
                self.c_x1 = int(arr[1])
                self.c_y1 = int(arr[2])
                self.c_x2 = int(arr[3])
                self.c_y2 = int(arr[4])
                self.west_point = [int(arr[5]),int(arr[6])]
                self.east_point = [int(arr[7]),int(arr[8])]
                self.car_center_x = int(arr[9])
                self.car_center_y = int(arr[10])
                self.slope_car = float(arr[11])
                    
            if arr[0] == "F":
                self.f_x1 = int(arr[1])
                self.f_y1 = int(arr[2])
                self.f_x2 = int(arr[3])
                self.f_y2 = int(arr[4])

            if arr[0] == "T":
                self.t_x1 = int(arr[1])
                self.t_y1 = int(arr[2])
                self.t_x2 = int(arr[3])
                self.t_y2 = int(arr[4])
                self.tar_center_x = int(arr[5])
                self.tar_center_y = int(arr[6])
                self.slope_tar = float(arr[7])

            if arr[0] == "P":
                print "Car: (", str(self.c_x1), " ", str(self.c_y1), ") (", str(self.c_x2), " ", str(self.c_y2), ") . Center: (", str(self.car_center_x), ",", str(self.car_center_y), ")" 
                print "Target: (", str(self.t_x1), " ", str(self.t_y1), ") (", str(self.t_x2), " ", str(self.t_y2), ") . Center: (", str(self.tar_center_x), ",", str(self.tar_center_y), ")" 
                print "Frame: (", str(self.f_x1), " ", str(self.f_y1), ") (", str(self.f_x2), " ", str(self.f_y2), ") "
                print "Slope of Car: ", str(self.slope_car)
                print "Slope to Target", str(self.slope_tar)
                print ""

            if arr[0] == "Q":
                self.run_server = False
            
        print "Server Terminated"

if __name__ == '__main__':
    ip = "127.0.0.1"
    try:
        ip = sys.argv[1]
    except IndexError:
        print "No IP Given: using 127.0.0.1"
        
    server = UDP_SERVER(ip, 5005)
    server.run()

    print "DONE"