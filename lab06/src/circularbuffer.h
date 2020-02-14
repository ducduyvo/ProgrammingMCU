/*
 * circularbuffer.h
 *
 *  Created on: 9 Feb 2020
 *      Author: Joshua
 */
#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

template <typename T, unsigned int N>
class circularbuffer {
	/* Iterator not currently implemented for this container */
public:
	constexpr circularbuffer() : buffer{ 0 }, size_{ 0 }, tail{ buffer }, end{ buffer + N } {}

	/* Not a perfect constructor because the compiler will allow different types to be in the parameter pack if they CAN be cast to type T (e.g. double to uint8_t).
	 * It will give a warning though. */
	template <typename... Ts>
	circularbuffer(Ts const &... args) : buffer{ std::forward<Ts>(args)... }, size_{ sizeof...(Ts) }, tail{ buffer }, end{ buffer + N } {}

	unsigned int const size() const {
		return size_;
	}

	T& operator[](unsigned int const index) {
		if (index < N)
			return (tail + index) >= end ? *(tail + index - N) : *(tail + index);
		else
			return *tail; // throw exception or asset an error later
	}

	void push_back(T const t) {
		if (size_ < N)
			this->operator[](size_++) = t;
		else {
			*tail = t;
			if (++tail == end)
				tail = buffer;
		}
	}

	void push_front(T const t) {
		if (--tail < buffer)
			tail = end - 1;
		*tail = t;

		if (size_ < N)
			++size_;
	}

	/* Pop functions could easily be modified to return the removed values, but it would be incosistent behaviour with other C++ containers */
	void pop_back() {
		if (size_ > 0)
			--size_;
	}

	void pop_front() {
		if (size_ > 0) {
			--size_;
			if (++tail == end)
				tail = buffer;
		}
	}

	private:
	T buffer[N];
	std::size_t size_;
	T* tail; /* Points to the oldest datum, although that gets muddied if you use push_front() */
	T const * const end;
};

#endif /* CIRCULARBUFFER_H_ */
