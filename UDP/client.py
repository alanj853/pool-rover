import socket
import sys

class UDP_CLIENT:

    def __init__(self, ip, port):
        self.PORT = port
        self.IP_ADDR = ip
        self.run_client = True

        ## Initialise
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP

    def send(self,data):
        self.sock.sendto(data, (self.IP_ADDR, self.PORT))

if __name__ == '__main__':
    data = "HelloWorld"

    try:
        data = sys.argv[1]
    except IndexError:
        print "No Arg Entered"

    print "Sending " + data

    server = UDP_CLIENT("127.0.0.1", 5005)
    server.send(data)

    print "DONE"