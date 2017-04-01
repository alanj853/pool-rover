import socket
import sys

class UDP_SERVER:

    def __init__(self, ip, port):
        self.PORT = port
        self.IP_ADDR = ip
        self.run_server = True
        self.count = 0

        ## Initialise
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        self.sock.bind((self.IP_ADDR, self.PORT))

    def run(self):
        while self.run_server:
            data, addr = self.sock.recvfrom(1024) # buffer size is 1024 bytes
            print "received message:", data
            self.count = self.count + 1

            if data == "end" or self.count > 4:
                self.run_server = False
            
        print "Server Terminated"

if __name__ == '__main__':
    
    ip = "127.0.0.1"
    try:
        ip = sys.argv[1]
    except IndexError:
        print "No IP entered. Using 127.0.0.1"

    server = UDP_SERVER(ip, 5005)
    server.run()

    print "DONE"