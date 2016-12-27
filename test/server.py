#!/usr/bin/python

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import random

PORT_NUMBER = 8082

class myHandler(BaseHTTPRequestHandler):
	def log_message(self, format, *args):
		pass

	def do_GET(self):
		self.send_response(200)
		self.send_header("Content-type", "text/plain")
		self.end_headers()

		sample_output = [-80, 80, 1, 0, 0]
		if(random.random() % 2 > 0.5):
			sample_output[2] = 0

		print sample_output
		self.wfile.write(sample_output)
		return

try:
	server = HTTPServer(('', PORT_NUMBER), myHandler)
	print 'Started httpserver on port ' , PORT_NUMBER
	server.serve_forever()

except KeyboardInterrupt:
	print '^C received, shutting down the web server'
	server.socket.close()
