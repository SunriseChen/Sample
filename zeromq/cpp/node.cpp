#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <stdlib.h>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include <boost/algorithm/string.hpp>

#define BOOST_PYTHON_SOURCE
#include <boost/python.hpp>
#include <Boost/Import/python.cpp>

#include <Boost/Import/format.h>

#define ZMQ_HAS_RVALUE_REFS
#include <zmq.hpp>

#pragma comment(lib, "python33.lib")
#pragma comment(lib, "libzmq.lib")

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::python;

const string transport = "tcp";
//const string host = "127.0.0.1";
const int port = 1234;
const string endpoint = "127.0.0.1:1234";
const string address = "tcp://127.0.0.1:1234";

const int PROCESS_COUNT = 3;
const int TEST_BLOCK_SIZE = 1024;

template<typename T, size_t N>
size_t countof(T (&arr)[N])
{
	return std::extent<T[N]>::value;
}

default_random_engine g_randomEngine;

string Question()
{
	uniform_real<double> genDouble(0, 100);
	double x = genDouble(g_randomEngine);
	double y = genDouble(g_randomEngine);

	// C++ 11 写法如下：
	//const static vector<string> ops = { "+", "-", "*", "/" };
	// VC2012 暂时不支持上面的写法，所以只能用下面的写法：
	const static string rawData[] = { "+", "-", "*", "/" };
	const static vector<string> ops(rawData, rawData + countof(rawData));
	uniform_int<int> genInt(0, 3);
	string op = ops[genInt(g_randomEngine)];

	if (op == "/" && (-0.01 < y && y < 0.01))
	{
		y = 100;
	}

	return (format("%.2f %s %.2f") % x % op % y).str();
}

string Answer(const string &question)
{
	return (format("%f") % extract<double>(eval(question.c_str()))).str();
}

void Request1(int count)
{
	zmq::context_t context(1);
	zmq::socket_t client(context, ZMQ_REQ);
	client.connect(address.c_str());

	uniform_int<unsigned char> genByte(0, 255);
	for (int i = 0; i < 10; ++i)
	{
		string msgSend = Question();
		zmq::message_t msg(msgSend.size() + 1);
		memcpy(msg.data(), msgSend.c_str(), msg.size());
		client.send(msg);
		cout << "Send: " << msgSend << endl;
		if (!client.recv(&msg))
		{
			cout << "Receive error !!!" << endl;
		}
		string msgReceive(static_cast<char *>(msg.data()));
		cout << "Receive: " << msgReceive << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}

	zmq::message_t reply(5);
	memcpy(reply.data(), "exit", reply.size());
	client.send(reply);
	//client.recv();
}

void Reply1(int count)
{
	zmq::context_t context(1);
	zmq::socket_t server(context, ZMQ_REP);
	server.bind(address.c_str());

	zmq::message_t msgReceive;
	count = PROCESS_COUNT;
	while (count > 0)
	{
		server.recv(&msgReceive);
		string str(static_cast<char *>(msgReceive.data()));
		cout << "Receive: " << str << endl;
		if (str == "exit")
		{
			--count;
			cout << count << endl;
			zmq::message_t reply(3);
			memcpy(reply.data(), "ok", reply.size());
			server.send(reply);
		}
		else
		{
			str = Answer(str);
			zmq::message_t msgSend(str.size() + 1);
			memcpy(msgSend.data(), str.c_str(), msgSend.size());
			server.send(msgSend);
		}
	}
}

void Request(int count)
{
	zmq::context_t context(1);
	zmq::socket_t client(context, ZMQ_REQ);
	client.connect(address.c_str());

	uniform_int<unsigned char> genByte(0, 255);
	for (int i = 0; i < 10; ++i)
	{
		zmq::message_t msgSend(TEST_BLOCK_SIZE);
		generate_n(static_cast<unsigned char *>(msgSend.data()), msgSend.size(), [&]()
		{
			return genByte(g_randomEngine);
		});
		if (!client.send(msgSend))
		{
			cout << "Send error !!!" << endl;
		}
		zmq::message_t msgReceive;
		if (!client.recv(&msgReceive))
		{
			cout << "Receive error !!!" << endl;
		}
		else if (memcmp(msgSend.data(), msgReceive.data(), msgSend.size()) != 0)
		{
			cout << "Receive Data error !!!" << endl;
		}
		this_thread::sleep_for(chrono::milliseconds(10));
	}

	zmq::message_t reply(5);
	memcpy(reply.data(), "exit", reply.size());
	client.send(reply);
	//client.recv();
}

void Reply(int count)
{
	zmq::context_t context(1);
	zmq::socket_t server(context, ZMQ_REP);
	server.bind(address.c_str());

	zmq::message_t msg;
	while (count > 0)
	{
		server.recv(&msg);
		string str(static_cast<char *>(msg.data()));
		if (str == "exit")
		{
			--count;
			cout << count << endl;
			zmq::message_t reply(3);
			memcpy(reply.data(), "ok", reply.size());
			server.send(reply);
		}
		else
		{
			server.send(msg);
		}
	}
}

void Publish1(int count)
{

}

void Subscribe1(int count)
{

}

void Publish(int count)
{

}

void Subscribe(int count)
{

}

void Ventilator1(int count)
{

}

void Worker1(int count)
{

}

void Sink1(int count)
{

}

void Ventilator(int count)
{

}

void Worker(int count)
{

}

void Sink(int count)
{

}

typedef void (*Func)(int);

Func GetFunc(const string &funcName)
{
	typedef map<string, Func> FuncMap;
	const static FuncMap::value_type rawData[] = {
		FuncMap::value_type("req1", Request1),
		FuncMap::value_type("rep1", Reply1),
		FuncMap::value_type("req", Request),
		FuncMap::value_type("rep", Reply),
		FuncMap::value_type("pub1", Publish1),
		FuncMap::value_type("sub1", Subscribe1),
		FuncMap::value_type("pub", Publish),
		FuncMap::value_type("sub", Subscribe),
		FuncMap::value_type("ventilator1", Ventilator1),
		FuncMap::value_type("worker1", Worker1),
		FuncMap::value_type("sink1", Sink1),
		FuncMap::value_type("Ventilator", Ventilator),
		FuncMap::value_type("Worker", Worker),
		FuncMap::value_type("Sink", Sink),
	};
	const static FuncMap funcMap(rawData, rawData + countof(rawData));

	auto found = funcMap.find(funcName);
	if (found != funcMap.end())
	{
		return found->second;
	}

	return nullptr;
}

void StartThread(Func func, int threadCount)
{
	const int count = threadCount * PROCESS_COUNT;

	vector<thread> threads;
	for (int i = 0; i < threadCount; ++i)
	{
		cout << "thread: " << i << endl;
		threads.push_back(thread(func, count));
		this_thread::sleep_for(chrono::milliseconds(100));
	}

	for (auto &t : threads)
	{
		t.join();
	}
}

int main(int argc, char *argv[])
{
	g_randomEngine.seed(time(0));
	if (argc > 1)
	{
		string mode = argv[1];
		to_lower(mode);
		int threads = 1;
		if (argc > 2)
		{
			threads = atoi(argv[2]);
		}

		auto func = GetFunc(threads > 1 ? mode : mode + "1");
		if (!func)
		{
			return 1;
		}

		Py_Initialize();

		if (threads > 1)
		{
			if (mode == "req" || mode == "sub" || mode == "worker")
			{
				StartThread(func, threads);
			}
			else
			{
				func(threads * PROCESS_COUNT);
			}
		}
		else
		{
			func(1);
		}

		Py_Finalize();

		system("pause");
	}

	return 0;
}
