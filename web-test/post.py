#!/usr/bin/env python3

import cgi, cgitb
cgitb.enable()

input_data = cgi.FieldStorage()

print("Content-Type: text/html", end="\r\n\r\n") # HTML is following
print("<h1>Addition Results</h1>")
try:
    num1 = int(input_data["num1"].value)
    num2 = int(input_data["num2"].value)
except:
    print("Sorry, the script cannot turn your inputs into numbers (integers).")
    raise SystemExit(0)
print("<ul>")
print("<li>num 1: {0}</li>".format(num1))
print("<li>num 2: {0}</li>".format(num2))
print("</ul>")
print("Addition: {0} + {1} = {2}".format(num1, num2, num1 + num2))
