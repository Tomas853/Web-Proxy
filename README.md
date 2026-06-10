## Web Proxy
A web proxy application that sits in between and relays HTTP requests between a server and a client. 

### Full Documentation

Refer to `pa4_Gebrewold.pdf` on this repository for a detailed documentation of the application. It contains brief description, architecture and general operations of how the software operates

### Installation

Follow the commands below only on linux terminal or windows terminal with linux support

Here are the instructions where required files and commands are stored
`Makefile`
    This is the makefile that builds the proxy program.  Type `make`
    to build your solution, or `make clean` followed by `make` for a
    fresh build. 

`port-for-user.pl`
    Generates a random port for a particular user
    usage: `./port-for-user.pl <userID>`

`free-port.sh`
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: `./free-port.sh`

`tiny`
    Tiny Web server from the CSAPP textbook. Using linux nwtwork APIs, it relays client-server request forwarding



