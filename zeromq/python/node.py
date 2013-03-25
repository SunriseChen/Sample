#! /usr/bin/env python
# -*- coding: utf-8 -*-

import zmq
import os, sys, time
from datetime import datetime
import random


transport = 'tcp'
host = '127.0.0.1'
port = 1234
endpoint = '%s:%d' % (host, port)
address = '%s://%s' % (transport, endpoint)
TEST_BLOCK_SIZE = 1024


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


#def question():
#	return os.urandom(TEST_BLOCK_SIZE)
#
#
#def answer(question):
#	from hashlib import md5
#	return md5(question).digest()


def req1():
	context = zmq.Context()
	client = context.socket(zmq.REQ)
	client.connect(address)

	for i in xrange(0, 10):
		msg_send = question()
		client.send(msg_send)
		print('Send: %s' % msg_send)
		msg_recv = client.recv()
		print('Receive: %s' % msg_recv)
		time.sleep(1)


def rep1():
	context = zmq.Context()
	server = context.socket(zmq.REP)
	server.bind(address)

	while True:
		msg = server.recv()
		print('Receive: %s' % msg)
		server.send(answer(msg))


def req(count):
	context = zmq.Context()
	client = context.socket(zmq.REQ)
	client.connect(address)

	for i in xrange(0, count):
		msg_send = os.urandom(TEST_BLOCK_SIZE)
		client.send(msg_send)
		if len(msg_send) != TEST_BLOCK_SIZE:
			print('Send error !!!')
		msg_recv = client.recv()
		if msg_send != msg_recv:
			print('Receive error !!!')
		time.sleep(0.0001 * count)


def rep():
	context = zmq.Context()
	server = context.socket(zmq.REP)
	server.bind(address)

	while True:
		msg = server.recv()
		server.send(msg)


def pub(count):
	context = zmq.Context()
	publisher = context.socket(zmq.PUB)
	publisher.bind(address)

	while True:
		msg = str(datetime.now())
		publisher.send(msg)
		print('Send: %s' % msg)
		time.sleep(1.0 / count)


def sub(count):
	context = zmq.Context()
	subscriber = context.socket(zmq.SUB)
	subscriber.connect(address)

	msg_filter = str(datetime.today().year)
	subscriber.setsockopt(zmq.SUBSCRIBE, msg_filter)

	for i in xrange(0, count):
		msg = subscriber.recv()
		print('Receive: %s' % msg)


def ventilator(count):
	context = zmq.Context()
	sender = context.socket(zmq.PUSH)
	sender.bind('%s://*:%d' % (transport, port))

	while True:
		msg = question()
		sender.send(msg)
		#print('Send: %s' % msg)
		time.sleep(1.0 / count)


def worker(count):
	context = zmq.Context()
	receiver = context.socket(zmq.PULL)
	receiver.connect('%s://localhost:%d' % (transport, port))

	sender = context.socket(zmq.PUSH)
	sender.connect('%s://localhost:%d' % (transport, port + 1))

	for i in xrange(0, count / 3):
		msg = receiver.recv()
		#print('Receive: %s' % msg)
		time.sleep(3.0 / count)
		sender.send('%s = %s' % (msg, answer(msg)))


def sink():
	context = zmq.Context()
	receiver = context.socket(zmq.PULL)
	receiver.bind('%s://*:%d' % (transport, port + 1))

	while True:
		msg = receiver.recv()
		#print('Receive: %s' % msg)


def start_thread(func, count):
	import threading

	threads = []
	for t in xrange(0, count):
		print('thread: %d' % t)
		thread = threading.Thread(target=func, args=(count,))
		thread.start()
		threads.append(thread)
		time.sleep(1.0 / count)

	for thread in threads:
		thread.join()


def main():
	random.seed()
	args = len(sys.argv)
	if args > 1:
		mode = sys.argv[1].lower()
		threads = 1
		if args > 2:
			threads = int(sys.argv[2])

		func = mode if threads > 1 else (mode + '1')
		func = getattr(sys.modules[__name__], func)

		if threads > 1 and (mode == 'req' or mode == 'sub' or mode == 'worker'):
			start_thread(func, threads)
		elif mode == 'pub' or mode == 'ventilator':
			func(threads)
		else:
			func()


if __name__ == '__main__':
	main()
