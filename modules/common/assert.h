/*
 * assert.h
 *
 *  Created on: 2014年3月30日
 *      Author: Feng
 */

#ifndef __ASSERT_H__
#define __ASSERT_H__

#include "macro.h"
#include "type.h"



#define checkParams(...) CheckAndParams(__VA_ARGS__)

#define CheckAndParams(...) \
		cires::Assert(__FILE__, __FUNC__, __LINE__).\
		checkAndParams(MAKE_PARAMS(__VA_ARGS__))
#define CheckOrParams(...) \
		cires::Assert(__FILE__, __FUNC__, __LINE__).\
		checkOrParams(MAKE_PARAMS(__VA_ARGS__))


namespace cires{


class Assert
{
	public:
		Assert(const string& file, const string& func, const int line)
			: _m_file(file), _m_func(func), _m_line(line)
		{
		}

		template <typename... CheckListType>
		bool checkAndParams(const CheckListType&... args)
		{
			bool hasdata = false;
			stringstream ss;
			ss << "Assersion \"";
			bool result = _checkAndParams(hasdata, ss, args...);
			ss << "\" failed in function '" << _m_func << "' of file '" << _m_file
			   << "' line '" << _m_line << "'.\n";
			if (!result)
				throw invalid_argument(ss.str());

			return result;
		}

		bool checkAndParams()
		{
			return true;
		}


		template <typename... CheckListType>
		bool checkOrParams(const CheckListType&... args)
		{
			bool hasdata = false;
			stringstream ss;
			ss << "Assersion \"";
			bool result = _checkOrParams(hasdata, ss, args...);
			ss << "\" failed in function '" << _m_func << "' of file " << _m_file
			   << "' line '" << _m_line << "'.";
			if (!result)
				throw invalid_argument(ss.str());

			return result;
		}

		bool checkOrParams()
		{
			return true;
		}


	private:
		template <typename ...CheckListType>
		bool _checkAndParams(bool& indicator, stringstream& ss, const bool& b, const string& msg, const CheckListType&... args) const
		{
			if (!b) {
				if (indicator) ss << " && ";
				ss << msg;
				indicator = true;
			}
			return _checkAndParams(indicator, ss, args...) && b;
		}

		bool _checkAndParams(bool& indicator, stringstream& ss) const
		{
			return true;
		}

		template <typename ...CheckListType>
		bool _checkOrParams(bool& indicator, stringstream& ss, const bool& b, const string& msg, const CheckListType&... args) const
		{
			if (!b) {
				if (indicator) ss << " || ";
				ss << msg;
				indicator = true;
			}
			return _checkOrParams(indicator, ss, args...) || b;
		}

		bool _checkOrParams(bool& indicator, stringstream& ss) const
		{
			return false;
		}

	private:
		string     _m_file;
		string     _m_func;
		int        _m_line;
};

inline
bool is_empty_string(const char* str)
{
	if (str == nullptr)
		return true;
	else
		return std::strlen(str) == 0;
}

inline
bool is_empty_string(nullptr_t str)
{
	return true;
}

inline
bool is_empty_string(const string& str)
{
	return str.empty();
}

}


#define RuntimeCheck(x, msg)                                          \
	if (!(x)) {                                                       \
		std::stringstream ss;                                         \
		ss << "Runtime assert: \"" << #x << "\" failed in function '" \
		   << __FUNC__ << "' of file '" << __FILE__ << "' line '"     \
		   << __LINE__ << "'.\n" ;                                    \
		if (!cires::is_empty_string(msg)) ss << "Message: " << std::string(msg) << ".\n"; \
		throw std::runtime_error(ss.str());                           \
	}

#define BoundaryCheck(x, msg)                                         \
	if (!(x)) {                                                       \
		std::stringstream ss;                                         \
		ss <<"Boundary assert: \"" << #x << "\" failed in function '" \
		   << __FUNC__ << "' of file '" << __FILE__ << "' line '"     \
		   << __LINE__ << "'.\n";                                     \
		if (!cires::is_empty_string(msg)) ss << "Message: " << msg << ".\n"; \
		throw std::out_of_range(ss.str());                            \
	}


#endif /* __ASSERT_H__ */
