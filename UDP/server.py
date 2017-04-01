import socket
import sys

class UDP_SERVER:

    def __init__(self, ip, port):
        self.PORT = port
        self.IP_ADDR = ip
        self.run_server = True
        self.count = 0
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
                self.c_x1 = arr[1]
                self.c_y1 = arr[2]
                self.c_x2 = arr[3]
                self.c_y2 = arr[4]
            if arr[0] == "F":
                self.f_x1 = arr[1]
                self.f_y1 = arr[2]
                self.f_x2 = arr[3]
                self.f_y2 = arr[4]
            if arr[0] == "T":
                self.t_x1 = arr[1]
                self.t_y1 = arr[2]
                self.t_x2 = arr[3]
                self.t_y2 = arr[4]

            if arr[0] == "P":
                print "Car: (", self.c_x1, " ", self.c_y1, ") (", self.c_x2, " ", self.c_y2, ") "
                print "Target: (", self.t_x1, " ", self.t_y1, ") (", self.t_x2, " ", self.t_y2, ") "
                print "Frame: (", self.f_x1, " ", self.f_y1, ") (", self.f_x2, " ", self.f_y2, ") "
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