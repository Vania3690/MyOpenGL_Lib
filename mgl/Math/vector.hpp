#pragma once
#include <initializer_list>
#include <algorithm>

#include "mgl/exceptions.hpp"
DefineNewMGLException(VectorIndexOutOfBounds);

namespace mgl::math {
	template<typename T, size_t S>
	class basic_vector {
	protected:
		T data[S];
	public:
		basic_vector(basic_vector<T, S> const& other) {
			std::copy(other.begin(), other.end(), data);
		}
		basic_vector(basic_vector<T, S>&& other) {
			std::move(other.begin(), other.end(), data);
		}
		template <typename... Tail>
		basic_vector(typename std::enable_if<sizeof...(Tail) + 1 <= S, T>::type head = T(0),
					 Tail... tail) : data{head, T(tail)...} {}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<S_O <= S>::type>
		basic_vector(basic_vector<T_O, S_O> const& other) {
			std::copy(other.begin(), other.end(), data);
			std::fill(data + S_O, data + S, T(0));
		}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<S_O <= S>::type>
		basic_vector(basic_vector<T_O, S_O>&& other) {
			std::move(other.begin(), other.end(), data);
			std::fill(data + S_O, data + S, T(0));
		}
		basic_vector(std::initializer_list<T> const& inputs) {
			if (inputs.size() > S)
				throw Exceptions::VectorException("Too many inputs.");
			std::copy(inputs.begin(), inputs.end(), data);
			std::fill(data + inputs.size(), data + S, T(0));
		}
		basic_vector(std::initializer_list<T>&& inputs) {
			if (inputs.size() > S)
				throw Exceptions::VectorException("Too many inputs.");
			std::move(inputs.begin(), inputs.end(), data);
			std::fill(data + inputs.size(), data + S, T(0));
		}
		basic_vector<T, S> const& operator=(basic_vector<T, S> const& other) {
			std::copy(other.begin(), other.end(), data);
			return *this;
		}
		basic_vector<T, S> const& operator=(basic_vector<T, S>&& other) {
			std::move(other.begin(), other.end(), data);
			return *this;
		}

		T const& operator[](size_t index) const {
			if (index >= S)
				throw Exceptions::VectorException("There is no element with this index in the vector");
			return data[index];
		}
		T& operator[](size_t index) {
			if (index >= S)
				throw Exceptions::VectorException("There is no element with this index in the vector");
			return data[index];
		}
		T const& at(size_t index) const {
			return operator[](index);
		}
		T& at(size_t index) {
			return operator[](index);
		}

		size_t size() const {
			return S;
		}
		T const* begin() const {
			return data;
		}
		T const* end() const {
			return data + S;
		}
		bool empty() const {
			for (size_t i = 0; i < S; i++)
				if (data[i] != T(0))
					return false;
			return true;
		}

		template<typename = typename std::enable_if<S >= 0 && S <= 4>::type> T const& x() const { return data[0]; }
		template<typename = typename std::enable_if<S >= 1 && S <= 4>::type> T const& y() const { return data[1]; }
		template<typename = typename std::enable_if<S >= 2 && S <= 4>::type> T const& z() const { return data[2]; }
		template<typename = typename std::enable_if<S >= 3 && S <= 4>::type> T const& w() const { return data[3]; }
		template<typename = typename std::enable_if<S >= 0 && S <= 4>::type> void x(T const& value) { data[0] = value; }
		template<typename = typename std::enable_if<S >= 1 && S <= 4>::type> void y(T const& value) { data[0] = value; }
		template<typename = typename std::enable_if<S >= 2 && S <= 4>::type> void z(T const& value) { data[0] = value; }
		template<typename = typename std::enable_if<S >= 3 && S <= 4>::type> void w(T const& value) { data[0] = value; }
	};

	template<typename T, size_t S, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	class basic_numeric_vector : public basic_vector<T, S> {
	protected:
		using basic_vector<T, S>::data;
	public:
		using basic_vector<T, S>::basic_vector;
		float length() const {
			T sum = T(0);
			for (size_t i = 0; i < S; i++)
				sum += data[i] * data[i];
			return std::sqrtf(sum);
		}
		template<typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
		void normalize() {
			auto l = length();
			for (size_t i = 0; i < S; i++)
				data[i] /= l;
		}
		
		template<typename..., typename T_O = T>
		typename std::enable_if<std::is_floating_point<T_O>::value, basic_numeric_vector<T_O, S>>::type const normalized() const {
			basic_numeric_vector<T_O, S> ret(*this);
			ret.normalize();
			return ret;
		}
		template<typename..., typename T_O = T>
		typename std::enable_if<!std::is_floating_point<T_O>::value, basic_numeric_vector<float, S>>::type const normalized() const {
			basic_numeric_vector<float, S> ret(*this);
			ret.normalize();
			return ret;
		}

		basic_numeric_vector<T, S> const& operator+=(basic_numeric_vector<T, S> const& other) {
			for (size_t i = 0; i < S; i++)
				data[i] += other.data[i];
			return *this;
		}
		basic_numeric_vector<T, S> const& operator-=(basic_numeric_vector<T, S> const& other) {
			for (size_t i = 0; i < S; i++)
				data[i] -= other.data[i];
			return *this;
		}
		basic_numeric_vector<T, S> const& operator*=(basic_numeric_vector<T, S> const& other) {
			for (size_t i = 0; i < S; i++)
				data[i] *= other.data[i];
			return *this;
		}
		basic_numeric_vector<T, S> const& operator/=(basic_numeric_vector<T, S> const& other) {
			for (size_t i = 0; i < S; i++)
				data[i] /= other.data[i];
			return *this;
		}
		basic_numeric_vector<T, S> const& operator*=(T const& q) {
			for (size_t i = 0; i < S; i++)
				data[i] *= q;
			return *this;
		}
		basic_numeric_vector<T, S> const& operator/=(T const& q) {
			for (size_t i = 0; i < S; i++)
				data[i] /= q;
			return *this;
		}

		basic_numeric_vector<T, S> const operator-() const {
			basic_numeric_vector<T, S> res;
			for (size_t i = 0; i < S; i++)
				res.data[i] = -data[i];
			return res;
		}
	};

	template<typename T, size_t S>
	class basic_homogeneous_numeric_vector : public basic_numeric_vector<T, S + 1> {
	protected:
		using basic_numeric_vector<T, S + 1>::data;
	public:
		template<typename... Tail>
		basic_homogeneous_numeric_vector(typename std::enable_if<sizeof...(Tail) + 1 <= S, T>::type head = T(0),
										 Tail... tail) : basic_numeric_vector<T, S + 1>(head, tail...) { data[S] = T(1); }
		template<typename... Tail>
		basic_homogeneous_numeric_vector(typename std::enable_if<sizeof...(Tail) == S, T>::type head = T(0),
										 Tail... tail) : basic_numeric_vector<T, S + 1>(head, tail...) {}
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<S_O <= S + 1>::type>
		basic_homogeneous_numeric_vector(basic_vector<T_O, S_O> const& other)
			: basic_numeric_vector<T, S + 1>(other) { data[S] = T(1); }
		template<typename T_O, size_t S_O, typename = typename std::enable_if<std::is_convertible<T_O, T>::value>::type, typename = typename std::enable_if<S_O <= S + 1>::type>
		basic_homogeneous_numeric_vector(basic_vector<T_O, S_O>&& other)
			: basic_numeric_vector<T, S + 1>(other) { data[S] = T(1); }
		basic_homogeneous_numeric_vector(T* inputs) : basic_numeric_vector<T, S + 1>(inputs) { data[S] = T(1); }
		basic_homogeneous_numeric_vector(std::initializer_list<T> const& inputs) : basic_numeric_vector<T, S + 1>(inputs) {
			data[S] = T(1);
		}
		basic_homogeneous_numeric_vector(std::initializer_list<T>&& inputs) : basic_numeric_vector<T, S + 1>(inputs) {
			data[S] = T(1);
		}
	};

	template<typename T, size_t S>
	bool operator==(basic_vector<T, S> const& v1, basic_vector<T, S> const& v2) {
		for (size_t i = 0; i < S; i++)
			if (v1[i] != v2[i])
				return false;
		return true;
	}
	template<typename T, size_t S>
	bool operator!=(basic_vector<T, S> const& v1, basic_vector<T, S> const& v2) {
		return !(v1 == v2);
	}

	template<typename T, size_t S>
	basic_numeric_vector<T, S> const operator+(basic_numeric_vector<T, S> const& v1, basic_numeric_vector<T, S> const& v2) {
		vector_basic<T, S> res = v1;
		return res += v2;
	}
	template<typename T, size_t S>
	basic_numeric_vector<T, S> const operator-(basic_numeric_vector<T, S> const& v1, basic_numeric_vector<T, S> const& v2) {
		vector_basic<T, S> res = v1;
		return res -= v2;
	}
	template<typename T, size_t S>
	basic_numeric_vector<T, S> const operator*(basic_numeric_vector<T, S> const& v1, basic_numeric_vector<T, S> const& v2) {
		vector_basic<T, S> res = v1;
		return res *= v2;
	}
	template<typename T, size_t S>
	basic_numeric_vector<T, S> const operator*(basic_numeric_vector<T, S> const& v, T const& q) {
		vector_basic<T, S> res = v;
		return res *= q;
	}
	template<typename T, size_t S>
	basic_numeric_vector<T, S> const operator*(T const& q, basic_numeric_vector<T, S> const& v) {
		vector_basic<T, S> res = v;
		return res *= q;
	}
	template<typename T, size_t S>
	basic_numeric_vector<T, S> const operator/(basic_numeric_vector<T, S> const& v1, basic_numeric_vector<T, S> const& v2) {
		vector_basic<T, S> res = v1;
		return res /= v2;
	}
	template<typename T, size_t S>
	basic_numeric_vector<T, S> const operator/(basic_numeric_vector<T, S> const& v, T const& q) {
		vector_basic<T, S> res = v;
		return res /= q;
	}

	template<typename T, size_t S>
	T const operator%(basic_numeric_vector<T, S> const& v1, basic_numeric_vector<T, S> const& v2) {
		T res = T(0);
		for (int i = 0; i < S; i++)
			res += v1[i] * v2[i];
		return res;
	}
	template<typename T, size_t S>
	T const dot(basic_numeric_vector<T, S> const& v1, basic_numeric_vector<T, S> const& v2) {
		return v1 % v2;
	}

	template<typename T>
	basic_numeric_vector<T, 3> const operator^(basic_numeric_vector<T, 3> const& v1, basic_numeric_vector<T, 3> const& v2) {
		return basic_numeric_vector<T, 3>(v1[1] * v2[2] - v2[1] * v1[2],
										  v1[2] * v2[0] - v2[2] * v1[0],
										  v1[0] * v2[1] - v2[0] * v1[1]);
	}
	template<typename T>
	basic_numeric_vector<T, 3> const cross(basic_numeric_vector<T, 3> const& v1, basic_numeric_vector<T, 3> const& v2) {
		return v1 ^ v2;
	}

	class vector2f : public basic_numeric_vector<float, 2u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector3f : public basic_numeric_vector<float, 3u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector4f : public basic_numeric_vector<float, 4u> { public: using basic_numeric_vector::basic_numeric_vector; };

	class vector2d : public basic_numeric_vector<double, 2u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector3d : public basic_numeric_vector<double, 3u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector4d : public basic_numeric_vector<double, 4u> { public: using basic_numeric_vector::basic_numeric_vector; };

	class vector2b : public basic_numeric_vector<uint8_t, 2u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector3b : public basic_numeric_vector<uint8_t, 3u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector4b : public basic_numeric_vector<uint8_t, 4u> { public: using basic_numeric_vector::basic_numeric_vector; };

	class vector2u : public basic_numeric_vector<uint32_t, 2u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector3u : public basic_numeric_vector<uint32_t, 3u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector4u : public basic_numeric_vector<uint32_t, 4u> { public: using basic_numeric_vector::basic_numeric_vector; };

	class vector2i : public basic_numeric_vector<int32_t, 2u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector3i : public basic_numeric_vector<int32_t, 3u> { public: using basic_numeric_vector::basic_numeric_vector; };
	class vector4i : public basic_numeric_vector<int32_t, 4u> { public: using basic_numeric_vector::basic_numeric_vector; };

	class vector : public vector3f { public: using vector3f::vector3f; };
	class vectorH : public basic_homogeneous_numeric_vector<float, 3u> { public: using basic_homogeneous_numeric_vector::basic_homogeneous_numeric_vector; };
}