#! /usr/bin/env python
# -*- coding: utf-8 -*-

import zmq
import sys, time
from datetime import datetime
import random


transport = 'tcp'
host = '127.0.0.1'
port = 1234
endpoint = '%s:%d' % (host, port)
address = '%s://%s' % (transport, endpoint)


def question():
	x = random.random() * 100
	y = random.random() * 100
	ops = ['+', '-', '*', '/']
	op = ops[random.randint(0, 3)]
	if op == '/' and '%.2f' % y == '0.00':
		y = 100
	return '%.2f %s %.2f' % (x, op, y)


def answer(question):
	return str(eval(question))


def req():
	context = zmq.Context()
	client = context.socket(zmq.REQ)
	client.connect(address)

	while True:
		msg_send = question()
		client.send(msg_send)
		print('Send: %s' % msg_send)
		msg_recv = client.recv()
		print('Receive: %s' % msg_recv)
		time.sleep(1)


def rep():
	context = zmq.Context()
	server = context.socket(zmq.REP)
	server.bind(address)

	while True:
		msg = server.recv()
		print('Receive: %s' % msg)
		server.send(answer(msg))


def pub():
	context = zmq.Context()
	publisher = context.socket(zmq.PUB)
	publisher.bind(address)

	while True:
		msg = str(datetime.now())
		publisher.send(msg)
		print('Send: %s' % msg)
		time.sleep(1)


def sub():
	context = zmq.Context()
	subscriber = context.socket(zmq.SUB)
	subscriber.connect(address)

	msg_filter = str(datetime.today().year)
	subscriber.setsockopt(zmq.SUBSCRIBE, msg_filter)

	while True:
		msg = subscriber.recv()
		print('Receive: %s' % msg)


def ventilator():
	context = zmq.Context()
	sender = context.socket(zmq.PUSH)
	sender.bind('%s://*:%d' % (transport, port))

	while True:
		msg = question()
		sender.send(msg)
		print('Send: %s' % msg)
		time.sleep(1)


def worker():
	context = zmq.Context()
	receiver = context.socket(zmq.PULL)
	receiver.connect('%s://localhost:%d' % (transport, port))

	sender = context.socket(zmq.PUSH)
	sender.connect('%s://localhost:%d' % (transport, port + 1))

	while True:
		msg = receiver.recv()
		print('Receive: %s' % msg)
		time.sleep(3)
		sender.send('%s = %s' % (msg, answer(msg)))


def sink():
	context = zmq.Context()
	receiver = context.socket(zmq.PULL)
	receiver.bind('%s://*:%d' % (transport, port + 1))

	while True:
		msg = receiver.recv()
		print('Receive: %s' % msg)


def main():
	random.seed()
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
		elif mode == 'ventilator':
			ventilator()
		elif mode == 'worker':
			worker()
		elif mode == 'sink':
			sink()


if __name__ == '__main__':
	main()
