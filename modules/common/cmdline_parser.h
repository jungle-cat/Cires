/*
 * cmdline_parser.h
 *
 *  Created on: 2014年4月21日
 *      Author: Feng
 */

#ifndef __CMDLINE_PARSER_H__
#define __CMDLINE_PARSER_H__

#include "type.h"

namespace cires {

class CommandLineParser
{
	public:

		//! the default constructor
		CommandLineParser(int argc, const char* const argv[],
		        const char* key_map);

		//! get parameter, you can choose: delete spaces in end and begin or not
		template<typename _Tp>
		_Tp get(const string& name, bool space_delete = true)
		{
			if (!has(name))
			{
				return _Tp();
			}
			string str = getString(name);
			return analyzeValue<_Tp>(str, space_delete);
		}

		//! print short name, full name, current value and help for all params
		void printParams();

		bool has(const string& keys);


	protected:
		map<string, vector<string> > o_data;
		map<string, vector<string> > m_data;
		string getString(const string& name);


		template<typename _Tp>
		_Tp analyzeValue(const string& str, bool space_delete = false);

		template<typename _Tp>
		static _Tp getData(const string& str)
		{
			_Tp res = _Tp();
			stringstream s1(str);
			s1 >> res;
			return res;
		}

		template<typename _Tp>
		_Tp fromStringNumber(const string& str); //the default conversion function for numbers

};

template<>
bool CommandLineParser::get<bool>(const string& name, bool space_delete);

template<>
std::string CommandLineParser::analyzeValue<string>(const string& str, bool space_delete);

template<>
int CommandLineParser::analyzeValue<int>(const string& str, bool space_delete);

template<>
unsigned int CommandLineParser::analyzeValue<unsigned int>(const string& str, bool space_delete);

template<>
uint64 CommandLineParser::analyzeValue<uint64>(const string& str, bool space_delete);

template<>
float CommandLineParser::analyzeValue<float>(const string& str, bool space_delete);

template<>
double CommandLineParser::analyzeValue<double>(const string& str, bool space_delete);


}

#endif /* __CMDLINE_PARSER_H__ */
