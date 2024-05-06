#!/usr/bin/python
# -*- coding: UTF-8 -*-
import  sys
import  os
import  urllib
import urllib2

url = "http://0.0.0.0:93/"
body_value = {
    "k1":"v1",
    "k2":"v2",
}
request = urllib2.Request(url, "[" + str(body_value) + ']')
#request.add_header(keys, headers[keys])
result = urllib2.urlopen(request ).read()

