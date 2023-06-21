# Webserv
## Methods
Methods implemented **GET**, **POST**, and **DELETE**.
## Error codes
## Headers not implemented
- Accept-Charsets: from Mozilla, ```Warning: Do not use this header. Browsers omit this header and servers should ignore it.```
- Authorization
- Content-Type: text/html
- Headers implemented
- Referer
- Retry-After
- User-Agent (Request)
- WWW-Authenticate
### Accept-Language
Return the languages that offers, by default en-US
```
Accept-Language: <language>
Accept-Language: *
Accept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5
```
### Allow
Return the methods availables for that location
```
Allow: GET, POST, HEAD
```
### Connection
```
Connection: keep-alive
Connection: close
```
### Content-Language
The Content-Language representation header is used to describe the language(s) intended for the audience
```
Content-Language: en-US
```
### Content-Length
The Content-Length header indicates the size of the message body, in bytes, sent to the recipient.
```
Content-Length: <length>
```
### Content-Location
On HTTP/1.1 201 Created, indicate the url
```
Content-Location: <url>

```
### Date
The Date general HTTP header contains the date and time at which the message originated.
```
Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
```
### Host
The Host request header specifies the host and port number of the server to which the request is being sent, port is optional.
```
Host: <host>:<port>
```
### Last-Modified
The Last-Modified response HTTP header contains a date and time when the origin server believes the resource was last modified.
```
Last-Modified: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
```
```struct stat.st_mtime``` will be used
### Location
The Location response header indicates the URL to redirect a page to. It only provides a meaning when served with a 3xx (redirection) or 201 (created) status response.
```
Location: <url>
```
### Server
The Server header describes the software used by the origin server that handled the request — that is, the server that generated the response.
```
Server: Webserv/1.0 (MacOS)
```
### Transfer-Encoding
- chunked

Data is sent in a series of chunks. The Content-Length header is omitted in this case and at the beginning of each chunk you need to add the length of the current chunk in hexadecimal format, followed by '\r\n' and then the chunk itself, followed by another '\r\n'.
## How requests work
```
Method Route Protocol
Header_1: header_1_args
Header_2: header_2_args
...
Header_n: header_n_args

Body
```
The size of the body will be ```Content-Length``` or chunked request. In any other case it will mean a 400 error, bad eequest
## How responses work
```
HTTP/1.1 StatusCode MessageCode
Header_1: header_1_args
Header_2: header_2_args
...
Header_n: header_n_args

Body
```

# Bibliography
https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/*