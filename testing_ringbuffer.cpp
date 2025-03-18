
#include "ring_buffer.h"
#include <gtest/gtest.h>
#include <string>
#include <ranges>
#include <algorithm>
//#include <print>
#include <thread>
#include <chrono>

using namespace std::string_literals;
using namespace std::chrono_literals;

struct person_t
{
	std::string name;
	int age;
};

//struct visitor_t
//{
//	void operator()(int const value)
//	{
//		std::print("{}", value);
//	}
//
//	void operator()(std::string_view value)
//	{
//		std::print("\"{}\"", value);
//	}
//
//	void operator()(person_t value)
//	{
//		std::print("{}:{}", value.name, value.age);
//	}
//};

// A ring buffer that can hold int, std::string and a user defined type person_t
// It is not thread safe and I don't think it should be. Such logic should be handled by the user/owner of the buffer in the same way as any container in STL
// Regarding performance; read and write is O(1) and iterating is O(N)
using ring_buffer = xo::collection::ring_buffer_t<std::variant<int, std::string, person_t>, 8>;
using ring_buffer_heap = std::unique_ptr<xo::collection::ring_buffer_t<int, 100'000>>;

//void dump_buffer(ring_buffer& buffer)
//{
//	std::string delim;
//	visitor_t visitor;
//	for (auto v : buffer)
//	{
//		std::print("{}", std::exchange(delim, ", "));
//		std::visit(visitor, v);
//	}
//
//	std::println();
//}

// Not 100% happy with this function, but it is all I could manage in the absence of size() on filter_views
// The goal is to show the ring_buffer's support for ranges, which it hopefully does.
size_t strings_only_count(ring_buffer& buffer)
{
	auto strings_only = [](ring_buffer::value_type const& v) {
		return std::holds_alternative<std::string>(v);
	};

	size_t i = 0;
	for (auto const& v : buffer | std::views::filter(strings_only))
	{
		++i;
	}

	return i;
}

//void log(std::string_view statement)
//{
//	std::println("{}: {}", std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()), statement);
//}

//void thread_body(std::stop_token stoken, ring_buffer& buffer)
//{
//	auto const tid = std::this_thread::get_id();
//	log(std::format("Thread {} started", tid));
//
//	std::uint64_t pushed_total = 0;
//	std::uint64_t popped_total = 0;
//	std::uint64_t pushed = 0;
//	std::uint64_t popped = 0;
//
//	int i = 0;
//	bool write = true;
//	while (!stoken.stop_requested())
//	{
//		if (write)
//		{
//			if (buffer.push(42))
//			{
//				++pushed;
//			}
//			++pushed_total;
//		}
//		else
//		{
//			if (buffer.pop())
//			{
//				++popped;
//			}
//			++popped_total;
//		}
//
//		if (++i % 77 == 0)
//		{
//			i = 0;
//			write = !write;
//		}
//	}
//
//	log(std::format("Thread {} done, pushed {}({}) and popped_total {}({})", tid, pushed_total, pushed, popped_total, popped));
//}

//void test_threading()
//{
//	ring_buffer buffer;
//
//	std::jthread thread1(thread_body, std::ref(buffer));
//	std::jthread thread2(thread_body, std::ref(buffer));
//
//	std::this_thread::sleep_for(1min);
//}

namespace
{
	TEST(RingBufferTest, size)
	{
		ring_buffer buffer;

		ASSERT_TRUE(buffer.empty());
		ASSERT_TRUE(buffer.push(1));
		ASSERT_EQ(buffer.size(), 1);
		ASSERT_TRUE(buffer.push("2"s));
		ASSERT_TRUE(buffer.push(3));
		ASSERT_EQ(buffer.size(), 3);
		ASSERT_TRUE(buffer.push("4"s));
		ASSERT_TRUE(buffer.push(5));
		ASSERT_TRUE(buffer.push("6"s));
		ASSERT_TRUE(buffer.push(7));
		ASSERT_EQ(buffer.size(), 7);
		ASSERT_FALSE(buffer.push(9));
		buffer.pop();
		buffer.pop();
		buffer.pop();
		buffer.pop();
		ASSERT_EQ(buffer.size(), 3);
		buffer.pop();
		buffer.pop();
		ASSERT_EQ(buffer.size(), 1);
		buffer.pop();
		ASSERT_TRUE(buffer.empty());
	}

	TEST(RingBufferTest, string_only_count)
	{
		ring_buffer buffer;

		ASSERT_TRUE(buffer.push(1));
		ASSERT_TRUE(buffer.push("2"s));
		ASSERT_TRUE(buffer.push(3));
		ASSERT_TRUE(buffer.push("4"s));
		ASSERT_TRUE(buffer.push(5));
		ASSERT_TRUE(buffer.push("6"s));
		ASSERT_EQ(strings_only_count(buffer), 3);
	}

	//TEST(RingBufferTest, count_if_string)
	//{
	//	ring_buffer buffer;

	//	ASSERT_TRUE(buffer.push(1));
	//	ASSERT_TRUE(buffer.push("2"s));
	//	ASSERT_TRUE(buffer.push(3));
	//	ASSERT_TRUE(buffer.push("4"s));
	//	ASSERT_TRUE(buffer.push(5));
	//	ASSERT_TRUE(buffer.push("6"s));

	//	ASSERT_EQ(std::count_if(buffer.begin(), buffer.end(), [](ring_buffer::value_type const& v)
	//		{
	//			return std::holds_alternative<std::string>(v);
	//		}), 3);
	//}

	TEST(RingBufferTest, all_types)
	{
		ring_buffer buffer;

		person_t p{ "sander", 51 };
		ASSERT_TRUE(buffer.push(p));
		ASSERT_TRUE(buffer.push("2"s));
		ASSERT_TRUE(buffer.push(3));
	}

	TEST(RingBufferTest, front_and_back)
	{
		xo::collection::ring_buffer_t<int, 8> buffer;

		ASSERT_FALSE(buffer.front());
		ASSERT_TRUE(buffer.push(42));
		ASSERT_EQ(buffer.front().value(), 42);
		ASSERT_EQ(buffer.back().value(), 42);

		ASSERT_TRUE(buffer.push(84));
		ASSERT_EQ(buffer.front().value(), 84);
		ASSERT_EQ(buffer.back().value(), 42);

		buffer.pop();
		ASSERT_EQ(buffer.front().value(), 84);
		ASSERT_EQ(buffer.back().value(), 84);
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

