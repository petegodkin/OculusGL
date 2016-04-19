#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "glm/gtc/quaternion.hpp"
#include <glm/glm.hpp>
#include <cstring>
#include <string>
#include <vector>
#include <map>

#if defined(_WIN32)
#  include <Windows.h>
#  define GLFW_EXPOSE_NATIVE_WIN32
#  define GLFW_EXPOSE_NATIVE_WGL
#endif

#define UP_DIR glm::vec3(0, 1, 0)
#define FACE_DIR glm::vec3(0, 0, -1)
#define FORWARD_DIR FACE_DIR
#define RIGHT_DIR glm::vec3(1, 0, 0)

namespace GUtils {
	void log(std::string toLog, const std::string file, int line) {
		std::cout << "{" << file << ":" << line << "} " << toLog << "\n";
	}

	std::string vecToString(glm::vec3 v)
	{
		//TODO, watch for issues here
		char* to = (char *)malloc(9);
		std::sprintf(to, "(%f, %f, %f)", v.x, v.y, v.z);

		std::string res = std::string(to);
		free(to);

		return res;
	}

	static float norm(glm::vec3 v)
	{
		return std::sqrt(std::pow(v.x, 2) +
						std::pow(v.y, 2) +
						std::pow(v.z, 2));
	}

	static glm::vec3 normVec(glm::vec3 v)
	{
		float norm = GUtils::norm(v);

		return glm::vec3(v.x / norm,
			v.y / norm,
			v.z / norm);
	}

	static glm::vec3 safeNorm(glm::vec3 v, glm::vec3 vIfZero)
	{
		float norm = GUtils::norm(v);

		if (norm > 0.001)
		{
			return GUtils::normVec(v);
		}
		else
		{
			return vIfZero;
		}
	}

	float clampToRange(float val, float eps, float a, float b) {
		if (a < b) {
			if (val - eps <= a)
				return a + eps;
			else if (val + eps >= b)
				return b - eps;
			else
				return val;
		}
		else {
			if (val - eps <= b)
				return b + eps;
			else if (val + eps >= a)
				return a - eps;
			else
				return val;
		}
	}

	float clampToRange(float val, float a, float b) {
		return GUtils::clampToRange(val, 0.0, a, b);
	}

	static glm::vec3 gauss(glm::mat3x4 A)
	{
		int n = 4;

		for (int i = 0; i < n; i++) {
			// Search for maximum in this column
			double maxEl = abs(A[i][i]);
			int maxRow = i;
			for (int k = i + 1; k<n; k++) {
				if (abs(A[k][i]) > maxEl) {
					maxEl = abs(A[k][i]);
					maxRow = k;
				}
			}

			// Swap maximum row with current row (column by column)
			for (int k = i; k < n + 1; k++) {
				double tmp = A[maxRow][k];
				A[maxRow][k] = A[i][k];
				A[i][k] = tmp;
			}

			// Make all rows below this one 0 in current column
			for (int k = i + 1; k < n; k++) {
				double c = -A[k][i] / A[i][i];
				for (int j = i; j < n + 1; j++) {
					if (i == j) {
						A[k][j] = 0;
					}
					else {
						A[k][j] += c * A[i][j];
					}
				}
			}
		}

		// Solve equation Ax=b for an upper triangular matrix A
		glm::vec3 x(n);
		for (int i = n - 1; i >= 0; i--) {
			x[i] = A[i][n] / A[i][i];
			for (int k = i - 1; k >= 0; k--) {
				A[k][n] -= A[k][i] * x[i];
			}
		}

		return x;
	}

	template<typename T>
	struct Opt {
		/// The underlying value that is stored.
			T value;
		/// Whether or not `value` has been set.
		bool _isSet;

		/// Creates an Optional type using `T`'s default constructor.
		Opt() : _isSet(false) {};
		/// Creates an Optional type using `defVaue` as a base value.
		Opt(T defValue) : _isSet(false) { value = defValue; }

		/// Sets `value` to `newValue`, and causes `isSet` to return true.
		void set(T newValue) {
			_isSet = true;
			value = newValue;
		}

		/// Whether or not `value` has been set.
		bool isSet() { return _isSet; }
		void clearSet() { _isSet = false; }
		/// Gets the underlying `value` of this Opt type.
		T& get() { return value; }
	};

	/// Represents a particular `property` in a file. It is equivalent to a
	/// string of the form "type:name=value".
	struct Property {
		/// The name of this property.
		std::string name;
		/// A string the represents the vlaue of thie property.
		std::string value;
		/// The type of this property as a string.
		std::string type;

		/// Creates an empty property.
		Property();
		Property(std::string value);
		/// Designated initializer.
		Property(std::string name, std::string value, std::string type);

		bool isDataType(std::string type);
		bool isBool();
		bool isDouble();
		bool isString();
		bool isVec3();
		bool isMat4();
		bool isArray();

		/// `Opt` is a sort of C++ way of handling optionally set types (e.g. like
		/// with the new Swift Language). Supports Eigen types. You might use this
		/// sort of like a pointer, where `NULL` means you point to nothing yet.
		/// So here, isSet() indicates whether you have set your "pointer" to
		/// something.

		Opt<bool> cachedBool;
		Opt<double> cachedDouble;
		Opt<glm::vec3> cachedVec;
		Opt<glm::mat4> cachedMat;
		Opt< std::vector<double> > cachedDoubleArray;
		void clearCaches();

		bool        asBool();
		float       asFloat(float def = 0.0f);
		double      asDouble(double def = 0.0);
		std::string asString();
		glm::vec3       asVec(glm::vec3 def = glm::vec3(0, 0, 0));
		glm::mat4       asMat(glm::mat4 def = glm::mat4());
		std::vector<double> asArray();
	};
}
//////////
// Operators
//////////

//
// The following gives a generic way of adding a string to any raw type.
//

#define DEFINE_STR_OP(op, Type) \
   std::string operator op (const std::string str, Type value);\
   std::string operator op (Type value, const std::string str)

#define IMPL_STR_OP(op, Type) \
   std::string operator op (const std::string str, Type value) { \
      std::ostringstream strFormatter; \
      strFormatter << value; \
      return str + strFormatter.str(); \
   } \
   std::string operator op (Type value, const std::string str) { \
      std::ostringstream strFormatter; \
      strFormatter << value; \
      return strFormatter.str() + str; \
   }

DEFINE_STR_OP(+, double);

template<typename T>
std::string operator+ (const std::string & str, std::vector<T> vec) {
	std::string vecAsStr = "[";
	for (int i = 0; i < (int)vec.size(); i++) {
		vecAsStr += std::string("") + vec[i];
		if (i < ((int)vec.size()) - 1)
			vecAsStr += ", ";
	}
	vecAsStr += "]";
	return str + vecAsStr;
}

template<typename T>
std::string operator+ (std::vector<T> vec, const std::string & str) {
	std::string vecAsStr = "[";
	for (int i = 0; i < vec.size(); i++) {
		vecAsStr += std::string("") + vec[i];
		if (i < vec.size() - 1)
			vecAsStr += ", ";
	}
	vecAsStr += "]";
	return vecAsStr + str;
}


//////////
// Assertions and Logging
//////////
#define UTIL_LOG(statement) \
	GUtils::log(std::string("") + statement, __FILE__, __LINE__)

#define LOG(statement) \
	UTIL_LOG(statement)

#define UTIL_ASSERT(cnd) {\
    if (!(cnd)) {\
        LOG("Assertion Failed: " #cnd);\
        exit(-1);\
    }\
}


