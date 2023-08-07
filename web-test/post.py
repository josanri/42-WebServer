#!/usr/bin/env python3

import cgi, cgitb
cgitb.enable()

input_data = cgi.FieldStorage()

print("Content-Type: text/html") # HTML is following
print("")                        # Leave a blank line
print("<h1>Addition Results</h1>")
try:
    # input_data 
    # num1=1&num2=2
    # print("<output>input_data: {0}</output>".format(input_data))
    num1 = int(input_data["num1"].value)
    num2 = int(input_data["num2"].value)
except:
    print("<output>Sorry, the script cannot turn your inputs into numbers (integers).</output>")
    raise SystemExit(1)
print("<output><ul></output>")
print("<output><li>num 1: {0}</li></output>".format(num1))
print("<output><li>num 2: {0}</li></output>".format(num2))
print("<output></ul></output>")
print("<output>Addition: {0} + {1} = {2}</output>".format(num1, num2, num1 + num2))
