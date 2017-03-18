#!/usr/bin/python

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import random
import json

PORT_NUMBER = 8082

class ControllerState(object):
	def __init__(self):
		self.START_BUTTON = 0
		self.Z_TRIG = 0
		self.B_BUTTON = 0
		self.A_BUTTON = 0
		self.R_TRIG = 0
		self.L_TRIG = 0
		self.X_AXIS = 0
		self.Y_AXIS = 0

class myHandler(BaseHTTPRequestHandler):
	def log_message(self, format, *args):
		pass

	def do_GET(self):
		self.send_response(200)
		self.send_header("Content-type", "text/plain")
		self.end_headers()

		sample_output = ControllerState()
		sample_output.X_AXIS = -80
		sample_output.Y_AXIS = 80
		if(random.random() % 2 > 0.5):
			sample_output.A_BUTTON = 1

		print json.dumps(sample_output.__dict__)
		self.wfile.write(json.dumps(sample_output.__dict__))
		return

try:
	server = HTTPServer(('', PORT_NUMBER), myHandler)
	print 'Started httpserver on port ' , PORT_NUMBER
	server.serve_forever()

except KeyboardInterrupt:
	print '^C received, shutting down the web server'
	server.socket.close()
