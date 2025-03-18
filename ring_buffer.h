
#ifndef XO_COLLECTION_RING_BUFFER
#define XO_COLLECTION_RING_BUFFER

#include <array>
#include <optional>
#include <variant>

namespace xo::collection
{
	template <typename T>
	struct ring_buffer_iterator_t
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using const_pointer = value_type const*;
		using const_reference = value_type const&;
		using pointer = value_type*;
		//using reference = value_type&;

		ring_buffer_iterator_t(pointer const base_ptr, size_t const array_capacity, pointer ptr)
			: base{ base_ptr }
			, capacity{ array_capacity }
			, current{ ptr }
		{
		}

		const_reference operator*() const
		{
			return *current;
		}

		const_pointer operator->() const
		{
			return current;
		}

		ring_buffer_iterator_t& operator++()
		{
			if (current + 1 == (base + capacity))
			{
				current = base;
			}
			else
			{
				++current;
			}

			return *this;
		}

		ring_buffer_iterator_t operator++(int)
		{
			auto* tmp = this;
			++(*this);
			return *tmp;
		}

		bool operator==(ring_buffer_iterator_t const& rhs) const
		{
			return current == rhs.current;
		}

		bool operator!=(ring_buffer_iterator_t const& rhs) const
		{
			return !(*this == rhs);
		}

		pointer const base;
		size_t capacity;
		pointer current;
	};

	template <typename T, size_t capacity>
	class ring_buffer_t
	{
	public:
		using value_type = T;
		using iterator_t = ring_buffer_iterator_t<value_type>;
		using buffer_t = std::array<value_type, capacity>;
		using index_t = int;

	public:
		ring_buffer_t()
			: buffer{ }
			, read{ 0 }
			, write{ 0 }
		{
		}

		[[nodiscard]] ring_buffer_t::iterator_t begin()
		{
			return ring_buffer_t::iterator_t(buffer.data(), buffer.size(), &buffer[read]);
		}

		[[nodiscard]] ring_buffer_t::iterator_t end()
		{
			return ring_buffer_t::iterator_t(buffer.data(), buffer.size(), &buffer[write]);
		}

		[[nodiscard]] bool empty() const
		{
			return write == read;
		}

		[[nodiscard]] std::size_t size() const
		{
			if (write >= read)
			{
				return write - read;
			}
			else
			{
				return buffer.size() - read + write;
			}
		}

		[[nodiscard]] bool push(ring_buffer_t::value_type const& value)
		{
			if ((write + 1) % buffer.size() == read)
			{
				// The buffer is full
				return false;
			}

			buffer[write] = value;
			write = (write + 1) % buffer.size();

			return true;
		}

		std::optional<ring_buffer_t::value_type> pop()
		{
			if (empty())
			{
				// The buffer is empty
				return std::nullopt;
			}

			auto const ret = buffer.at(read);

			read = (read + 1) % buffer.size();

			return ret;
		}

		[[nodiscard]] std::optional<ring_buffer_t::value_type> front() const
		{
			if (empty())
			{
				return std::nullopt;
			}
			else
			{
				auto const index = write == 0 ? buffer.size() - 1 : write - 1;
				return buffer.at(index);
			}
		}

		[[nodiscard]] std::optional<ring_buffer_t::value_type> back() const
		{
			if (empty())
			{
				return std::nullopt;
			}
			else
			{
				return buffer.at(read);
			}
		}

		template <typename Func>
		[[nodiscard]] auto operator | (Func&& f)
		{
			return f(buffer);
		}

	private:
		buffer_t buffer;
		index_t read;
		index_t write;
	};
}

#endif
