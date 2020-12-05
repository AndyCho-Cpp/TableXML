#pragma once
#include "CommonFunction.hpp"
#include <map>
namespace CommonHelper
{

	//using namespace CommonHelper;

	enum class eValueCount : char
	{
		N, None, One, OneOrMore, Two, TwoOrMore
	};

	struct sArgItem
	{
		sArgItem() { valType = eValueCount::None; }
		sArgItem(eValueCount valType, const char* key)
		{
			this->valType = valType;
			this->key = key;
		}
		sArgItem(eValueCount valType, const char* key, const char *description)
			: sArgItem::sArgItem(valType, key)
		{
			if (description)
				this->description = description;
		}

		eValueCount valType;
		std::string key;
		std::string description;
		std::vector<std::string> values;
	};


	// Argument class
	// key(string) - value(string)
	//
	class ArgumentHandler
	{
		std::map<std::string, sArgItem> argsTable;
		std::map<std::string, sArgItem> argsParsed;
		std::vector<std::string> argsOrder;
		std::string usageDescription;

	public:

		ArgumentHandler()
		{

		}
		static const char* ValTypeToString(eValueCount val)
		{
			switch (val)
			{
			case eValueCount::N: return "0~n";
			case eValueCount::None: return "None";
			case eValueCount::One: return "One";
			case eValueCount::OneOrMore: return "One or more";
			case eValueCount::Two: return "Two";
			case eValueCount::TwoOrMore: return "Two or more";
			}
			return "";
		}
		bool Initialize(sArgItem *argList, int count, std::string *errMsg)
		{
			std::vector<sArgItem> vlist;
			for (int k = 0; k < count; k++)
				vlist.push_back(argList[k]);

			return Initialize(vlist, errMsg);
		}
		bool Initialize(std::vector<sArgItem> &argList, std::string *errMsg)
		{
			argsTable.clear();
			argsParsed.clear();
			argsOrder.clear();

			for (auto arg : argList)
			{
				if (arg.key.size() == 0)
				{
					if (errMsg)
						*errMsg = "Empty key";
					return false;
				}

				arg.key = UpperCaseString(arg.key);

				if (IsThisKeyExist_inMap(argsTable, arg.key))
				{
					if (errMsg)
						*errMsg = "Duplicated key error: " + arg.key;
					return false;
				}

				//if (arg.key[0] != '/')
				//	arg.key.insert(arg.key.begin(), '/');

				argsTable[arg.key] = arg;	//insert
				argsOrder.push_back(arg.key);
			}
			return true;
		}


		std::string &GetUsage()
		{
			usageDescription = " * * * * * Arguments Usage * * * * *\n\n";
			for (std::string key : argsOrder)
			{
				sArgItem item = argsTable[key];
				if (item.description.empty())
					continue;

				usageDescription += StringFormat1KB("%s", item.key.c_str());
				usageDescription += StringFormat1KB(" [ValueType: %s]", ValTypeToString(item.valType));
				if (item.description.size() > 0)
					usageDescription += StringFormat1KB("\n\t-> %s", item.description.c_str());
				usageDescription += "\n";
			}

			return usageDescription;
		}
	private:
		bool Parse(std::vector<std::string> &inputArgs, std::string *pErrMsg)
		{
			argsParsed.clear();
			for (auto &a : argsTable)
				a.second.values.clear();

			auto iter = argsTable.end();
			std::string key;
			while (inputArgs.size() > 0)
			{
				std::string key = *inputArgs.begin();
				inputArgs.erase(inputArgs.begin());
				iter = argsTable.find(UpperCaseString(key));
				if (iter == argsTable.end())
				{
					if (pErrMsg)
						*pErrMsg = "[" + key + "] is not a key argment.";
					return false;
				}


				while (inputArgs.size() > 0)
				{
					std::string value = *inputArgs.begin();
					if (IsThisKeyExist_inMap(argsTable, UpperCaseString(value)))
						break;	//another key begin

					inputArgs.erase(inputArgs.begin());
					iter->second.values.push_back(value);
				}

				argsParsed[UpperCaseString(key)] = iter->second;
			}


			// Validation Check
			for (auto iter : argsParsed)
			{
				switch (iter.second.valType)
				{
				case eValueCount::None:
					if (iter.second.values.size() > 0)
					{
						if (pErrMsg)
						{
							*pErrMsg = "[" + iter.second.key + "] key cannot have value. -> ";
							for (auto val : iter.second.values)
								*pErrMsg += val + " ";
						}
						return false;
					}
					break;

				case eValueCount::One:
					if (iter.second.values.size() == 0)
					{
						if (pErrMsg)
							*pErrMsg = "[" + iter.second.key + "] key must have a value.";
						return false;
					}
					else if (iter.second.values.size() > 1)
					{
						if (pErrMsg)
						{
							*pErrMsg = "[" + iter.second.key + "] key cannot have one more values -> ";
							for (auto val : iter.second.values)
								*pErrMsg += val + " ";
						}
						return false;
					}
					break;

				case eValueCount::OneOrMore:
					if (iter.second.values.size() == 0)
					{
						if (pErrMsg)
							*pErrMsg = "[" + iter.second.key + "] key must have a value.";
						return false;
					}
					break;

				case eValueCount::Two:
					if (iter.second.values.size() != 2)
					{
						if (pErrMsg)
						{
							*pErrMsg = "[" + iter.second.key + "] key must have two values. ";
							if (iter.second.values.size() > 0)
								*pErrMsg += "-> ";
							for (auto val : iter.second.values)
								*pErrMsg += val + " ";
						}
						return false;
					}
					break;

				case eValueCount::TwoOrMore:
					if (iter.second.values.size() < 2)
					{
						if (pErrMsg)
						{
							*pErrMsg = "[" + iter.second.key + "] key must have two more values. ";
							if (iter.second.values.size() > 0)
								*pErrMsg += "-> ";
							for (auto val : iter.second.values)
								*pErrMsg += val + " ";
						}
						return false;
					}
					break;
				}
			}
			return true;
		}
	public:
		bool Parse(std::string cmd, std::string *pErrMsg)
		{
			//Synctex
			//   move "this file.txt" "there is.txt"
			std::vector<std::string> inputArgs;
			if (StrSplitSentensWord(cmd.c_str(), &inputArgs, pErrMsg) == false)
				return false;

			return Parse(inputArgs, pErrMsg);
		}

		bool IsExist(std::string key)
		{
			key = UpperCaseString(key);
			return IsThisKeyExist_inMap(argsParsed, key);
		}
	private:
		const char *GetParsedValue(unsigned int No, std::string &key)
		{
			key = UpperCaseString(key);
			if (IsThisKeyExist_inMap(argsParsed, key) == false)
				return nullptr;
			if (argsParsed[key].values.size() > No)
				return argsParsed[key].values[No].c_str();
			return nullptr;
		}
	public:

		int ValueCount(std::string key)
		{
			key = UpperCaseString(key);
			auto iter = argsParsed.find(key);
			if (iter == argsParsed.end())
				return 0;
			return iter->second.values.size();
		}
		bool GetValue(unsigned int No, std::string key, std::string *out)
		{
			const char *o = GetParsedValue(No, key);
			if (o == nullptr)
				return false;

			if (out != nullptr)
				*out = o;

			return true;
		}
		bool GetValue(unsigned int No, std::string key, char *out, int size)
		{
			const char *o = GetParsedValue(No, key);
			if (o == nullptr)
				return false;

			if (out != nullptr)
				strncpy_s(out, size, o, size - 1);

			return true;
		}
		bool GetValue(unsigned int No, std::string key, int *out)
		{
			const char *o = GetParsedValue(No, key);
			if (o == nullptr)
				return false;

			return TryParseInt(o, out);
		}
		bool GetValue(unsigned int No, std::string key, double *out)
		{
			const char *o = GetParsedValue(No, key);
			if (o == nullptr)
				return false;

			return TryParseDouble(o, out);
		}
		bool GetValue(unsigned int No, std::string key, bool *out)
		{
			const char *o = GetParsedValue(No, key);
			if (o == nullptr)
				return false;

			return TryParseBool(o, out);
		}
	};
};