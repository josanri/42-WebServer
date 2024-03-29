#ifndef HTTP_ERROR_CODE_HPP
# define HTTP_ERROR_CODE_HPP

# define RESPONSE_CODE__CONTINUE                  "100 Continue"
# define RESPONSE_CODE__SWITCHING_PROTOCOL        "101 Switching Protocol"
# define RESPONSE_CODE__PROCESSING                "102 Processing"
# define RESPONSE_CODE__EARLY_HINTS               "103 Early Hints"

# define RESPONSE_CODE__OK                        "200 OK"
# define RESPONSE_CODE__CREATED                   "201 Created"
# define RESPONSE_CODE__ACCEPTED                  "202 Accepted"
# define RESPONSE_CODE__NON_AUTHORITATIVE_INFO    "203 Non-Authoritative Information"
# define RESPONSE_CODE__NO_CONTENT                "204 No Content"
# define RESPONSE_CODE__RESET_CONTENT             "205 Reset Content"
# define RESPONSE_CODE__PARTIAL_CONTENT           "206 Partial Content"
# define RESPONSE_CODE__MULTI_STATUS              "207 Multi-Status"
# define RESPONSE_CODE__ALREADY_REPORTED          "208 Already Reported"
# define RESPONSE_CODE__IM_USED                   "226 IM Used"

# define RESPONSE_CODE__MULTIPLE_CHOICE           "300 Multiple Choice"
# define RESPONSE_CODE__MOVED_PERMANENTLY         "301 Moved Permanently"
# define RESPONSE_CODE__FOUND                     "302 Found"
# define RESPONSE_CODE__SEE_OTHER                 "303 See Other"
# define RESPONSE_CODE__NOT_MODIFIED              "304 Not Modified"
# define RESPONSE_CODE__USE_PROXY                 "305 Use Proxy"
# define RESPONSE_CODE__TEMPORARY_REDIRECT        "307 Temporary Redirect"
# define RESPONSE_CODE__PERMANENT_REDIRECT        "308 Permanent Redirect"

# define RESPONSE_CODE__BAD_REQUEST               "400 Bad Request"
# define RESPONSE_CODE__UNAUTHORIZED              "401 Unauthorized"
# define RESPONSE_CODE__FORBIDDEN                 "403 Forbidden"
# define RESPONSE_CODE__NOT_FOUND                 "404 Not Found"
# define RESPONSE_CODE__METHOD_NOT_ALLOWED        "405 Method Not Allowed"
# define RESPONSE_CODE__NOT_ACCEPTABLE            "406 Not Acceptable"
# define RESPONSE_CODE__PROXY_AUTHENTICATION_REQ  "407 Proxy Authentication Required"
# define RESPONSE_CODE__REQUEST_TIMEOUT           "408 Request Timeout"
# define RESPONSE_CODE__CONFLICT                  "409 Conflict"
# define RESPONSE_CODE__GONE                      "410 Gone"
# define RESPONSE_CODE__LENGTH_REQUIRED           "411 Length Required"
# define RESPONSE_CODE__PRECONDITION_FAILED       "412 Precondition Failed"
# define RESPONSE_CODE__PAYLOAD_TOO_LARGE         "413 Payload Too Large"
# define RESPONSE_CODE__URI_TOO_LONG              "414 URI Too Long"
# define RESPONSE_CODE__UNSUPPORTED_MEDIA_TYPE    "415 Unsupported Media Type"
# define RESPONSE_CODE__REQUESTED_RANGE_NOT_SAT   "416 Requested Range Not Satisfiable"
# define RESPONSE_CODE__EXPECTATION_FAILED        "417 Expectation Failed"
# define RESPONSE_CODE__IM_A_TEAPOT               "418 I'm a teapot"
# define RESPONSE_CODE__MISREDIRECT_REQUEST       "421 Misredirect Request"
# define RESPONSE_CODE__UNPROCESSABLE_ENTITY      "422 Unprocessable Entity"
# define RESPONSE_CODE__LOCKED                    "423 Locked"
# define RESPONSE_CODE__FAILED_DEPENDENCY         "424 Failed Dependency"
# define RESPONSE_CODE__UPGRADE_REQUIRED          "426 Upgrade Required"
# define RESPONSE_CODE__PRECONDITION_REQUIRED     "428 Precondition Required"
# define RESPONSE_CODE__TOO_MANY_REQUESTS         "429 Too Many Requests"
# define RESPONSE_CODE__REQUEST_HEADER_FIELDS_TOO_LARGE   "431 Request Header Fields Too Large"
# define RESPONSE_CODE__UNAVAILABLE_FOR_LEGAL_REASONS     "451 Unavailable For Legal Reasons"

# define RESPONSE_CODE__INTERNAL_SERVER_ERROR     "500 Internal Server Error"
# define RESPONSE_CODE__NOT_IMPLEMENTED           "501 Not Implemented"
# define RESPONSE_CODE__BAD_GATEWAY               "502 Bad Gateway"
# define RESPONSE_CODE__SERVICE_UNAVAILABLE       "503 Service Unavailable"
# define RESPONSE_CODE__GATEWAY_TIMEOUT           "504 Gateway Timeout"
# define RESPONSE_CODE__HTTP_VERSION_NOT_SUPPORTED       "505 HTTP Version Not Supported"
# define RESPONSE_CODE__VARIANT_ALSO_NEGOTIATES   "506 Variant Also Negotiates"
# define RESPONSE_CODE__INSUFFICIENT_STORAGE      "507 Insufficient Storage"
# define RESPONSE_CODE__LOOP_DETECTED            "508 Loop Detected"
# define RESPONSE_CODE__NOT_EXTENDED              "510 Not Extended"
# define RESPONSE_CODE__NETWORK_AUTHENTICATION_REQ   "511 Network Authentication Required"

#endif