#! /usr/bin/env python
# -*- coding: utf-8 -*-

import zmq
import sys, time
from datetime import datetime


transport = 'tcp'
endpoint = '127.0.0.1:1234'
address = '%s://%s' % (transport, endpoint)


def req():
	context = zmq.Context()
	socket = context.socket(zmq.REQ)
	socket.connect(address)

	while True:
		msg_send = str(datetime.now())
		socket.send(msg_send)
		print('Send: %s' % msg_send)
		msg_recv = socket.recv()
		print('Receive: %s' % msg_recv)
		time.sleep(1)


def rep():
	context = zmq.Context()
	socket = context.socket(zmq.REP)
	socket.bind(address)

	while True:
		msg = socket.recv()
		print('Receive: %s' % msg)
		socket.send(msg + ' received')


def pub():
	context = zmq.Context()
	socket = context.socket(zmq.PUB)
	socket.bind(address)

	while True:
		msg = str(datetime.now())
		socket.send(msg)
		print('Send: %s' % msg)
		time.sleep(1)


def sub():
	context = zmq.Context()
	socket = context.socket(zmq.SUB)
	socket.connect(address)

	while True:
		print('loop')
		msg = socket.recv()
		print('Receive: %s' % msg)
		socket.send(msg + ' received')


def main():
	if len(sys.argv) > 1:
		mode = sys.argv[1].lower()
		if mode == 'req':
			req()
		elif mode == 'rep':
			rep()
		elif mode == 'pub':
			pub()
		elif mode == 'sub':
			sub()


if __name__ == '__main__':
	main()
