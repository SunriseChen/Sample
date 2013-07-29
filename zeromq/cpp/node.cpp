#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <stdlib.h>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include <boost/algorithm/string.hpp>

#pragma comment(lib, "python27.lib")
#define BOOST_PYTHON_SOURCE
#include <boost/python.hpp>
#include <Boost/Import/python.cpp>

#include <Boost/Import/format.h>

#define ZMQ_HAS_RVALUE_REFS
#include <zmq.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::python;

const string transport = 'tcp';
//const string host = '127.0.0.1';
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

}

void Reply1(int count)
{

}

void Request(int count)
{
	zmq::context_t context(1);
	zmq::socket_t client(context, ZMQ_REQ);
	client.connect(address);

	uniform_int<unsigned char> genByte(0, 255);
	zmq::message_t msgSend(TEST_BLOCK_SIZE), msgReceive;
	for (int i = 0; i < 10; ++i)
	{
		generate_n(msgSend.data(), msgSend.size(), [&]() { return genByte(g_randomEngine); });
		client.send(msgSend);
		if (msgSend.size() != TEST_BLOCK_SIZE)
		{
			cout << "Send error !!!" << endl;
		}
		client.recv(&msgReceive);
		if (msgSend != msgReceive)
		{
			cout << "Receive error !!!" << endl;
		}
		this_thread::sleep_for(chrono::milliseconds(10));
	}

	client.send('exit')
	//client.recv()
}

void Reply(int count)
{
	zmq::context_t context(1);
	zmq::socket_t server(context, ZMQ_REP);
	server.bind(address);

	zmq::message_t msg;
	while (count > 0)
	{
		server.recv(&msg);
		if (msg == "exit")
		{
			--count;
			cout << count << endl;
			server.send("ok");
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

typedef void FuncType(int);
typedef std::function<FuncType> Func;

Func GetFunc(const string &funcName)
{
	typedef map<string, FuncType*> FuncMap;
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
		return Func(found->second);
	}

	return nullptr;
}

void StartThread(Func &func, int threadCount)
{
	const int count = threadCount * PROCESS_COUNT;

	vector<std::shared_ptr<thread>> threads;
	//vector<thread> threads;
	for (int i = 0; i < threadCount; ++i)
	{
		cout << "thread: " << i << endl;
		std::shared_ptr<thread> t(new thread(func, count));
		threads.push_back(t);
		//threads.push_back(thread(func, count));
		this_thread::sleep_for(chrono::milliseconds(100));
	}

	for (auto &t : threads)
	{
		if (t)
		{
			t->join();
		}
		//t.join();
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
	}

	return 0;
}
