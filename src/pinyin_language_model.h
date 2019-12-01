#pragma once
#ifndef __PINYIN_LANGUAGE_MODEL_H__
#define __PINYIN_LANGUAGE_MODEL_H__
#include "pinyin_utils.h"
#include <unordered_map>
#include <fstream>

class BigramLanguageModel
{
	typedef std::unordered_map<std::string, int> NGrams;
public:
	BigramLanguageModel()
	{
	}

	/*
		During load, the following cases are not handled for fast loading:
		1. trim space during each steps
			for example:
			a. pin ya || 1000
		2. more than one whitespace between the two tokens of bi-gram
		3. missing cout
		It is the model trainer's duty to make sure:
			ui-gram: "token||1111"
			bi-gram:"token token||1111"
	*/
	void Load(const char *file_path)
	{
		const size_t MinValidLineLength = 3;
		if (file_path != nullptr)
		{
			std::ifstream file(file_path);
			std::string line;
			while (!file.eof())
			{
				std::getline(file, line);
				// skip comments
				if (line[0] == '#' || line.length() < MinValidLineLength)
				{
					continue;
				}
				ParseLine(line);
			}
		}
	}

	double Perplexity(const StringList &pyList, size_t rawStringLength = 0)
	{
		std::string w1;
		w1.reserve(30);
		double p = 1.0; // probobility
		StringList::const_iterator iter = pyList.begin();
		w1 = pyList.front();
		p *= uigrams_[w1] * 1.0 / N_;
		for (iter++; iter != pyList.end(); iter++)
		{
			auto count_of_w1 = GetGramsCount(uigrams_, w1);
			const std::string &w2 = *iter;
			w1 += " ";
			w1 += w2;
			auto count_of_w1_w2 = GetGramsCount(bigrams_, w1);

			double p_w1_w2 = (K_ + count_of_w1_w2) / (count_of_w1 + K_ * uigrams_.size());
			p *= p_w1_w2;
			w1 = w2;
		}

		size_t noramlizedN = rawStringLength != 0 ?  (rawStringLength + sqrt( 1.0 * pyList.size())) / 1.5 : pyList.size();
		return pow(p, -1.0 / noramlizedN);
	}
private:

	static int GetGramsCount(const NGrams &grams, const std::string &gram)
	{
		auto iter = grams.find(gram);
		if (iter != grams.end())
		{
			return iter->second;
		}
		else
		{
			return 0;
		}
	}

	int ParseLine(const std::string &line)
	{
		auto first_splitter = line.find(splitter_);
		if (first_splitter == std::string::npos)
		{
			return -1;
		}

		auto second_splitter = line.find(splitter_, first_splitter + 1);
		if (second_splitter == std::string::npos)
		{
			return -1;
		}

		auto str_grams = line.substr(0, first_splitter);
		auto str_count = line.substr(second_splitter + 1);
		auto count = atoi(str_count.c_str());

		if (str_grams.find(' ') == std::string::npos)
		{
			uigrams_[str_grams] = count;
			N_ += count;
		}
		else
		{
			bigrams_[str_grams] = count;
		}

		return count;
	}

	char splitter_ = '|';
	NGrams uigrams_;
	NGrams bigrams_;
	size_t N_ = 0;
	double K_ = 1.0;
};
#endif