# DNSRelay
A project of Curriculum Practice of Computer Networks.
Provides following features:

1. Domain name query with local domain-name-IP table (DNS Server).
2. If the search result is a normal IP address, the address is returned to the client (Server Function).
3. If the domain name is not found in the table, a query is sent to the Internet DNS server and the result is returned to the client (Relay Function).
## Build
```
make full
```
## Usage
```
./dnsrelay [--update]
           [--debug ]
--update : update local cache dynamicly using LRU Algorithm
--debug  : output the header of the Message		   
```
