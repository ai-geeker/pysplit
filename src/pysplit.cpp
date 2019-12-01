#include <stdio.h>
#include "pinyin_utils.h"
#include "pinyin_language_model.h"
#include "pinyin_tire_tree.h"

void test_split(PinyinTireTree &pinyinTree, BigramLanguageModel &lm, const char *s, bool support_initials)
{
	std::list<StringList> ret;
	auto split_result = PinyinSpiltResult::Create(s, &pinyinTree.Root(), support_initials);
	printf("# %s\n", s);
	split_result->GetTopN(ret);

	for (auto &l : ret)
	{
		std::cout << ">: " << l;
		printf("| perplexity: %lg\n", lm.Perplexity(l, strlen(s)));
	}
}

void showUsage()
{
	printf("pysplit [-intials] test1 test2 test3 ...\n");
	printf("pysplit -h: help\n");
	printf("pysplit -intials(-i): support split by single intial\n");
}

int main(int nArgs, const char *vArgs[])
{
	DEFINE_CLOG;
	SET_CLOG(true);

	if (nArgs <= 1)
	{
		showUsage();
		return 0;
	}

	int test_string_start = 1;
	bool supportSplitBySingleIntitial = false;
	const char* model_file_path = "model/pinyin_lm.pkl";
	
	int current_arg_index = 1;
	const char *current_arg = vArgs[current_arg_index];

	while (current_arg[0] == '-')
	{
		CLOG << "model" << current_arg << std::endl;
		test_string_start++;
		const char * option = current_arg + 1;
		if (option[0] == 'i')
		{
			supportSplitBySingleIntitial = true;
		}
		else if (option[0] == 'h')
		{
			showUsage();
			return 1;
		}
		else if (strncmp(option, "lm", 2) == 0){
			CLOG << "language model" << std::endl;
			if (current_arg_index + 1 < nArgs)
				model_file_path = vArgs[current_arg_index + 1];
				current_arg_index++;
				test_string_start++;
		}
		current_arg_index++;
		current_arg = vArgs[current_arg_index];
	}

	PinyinTireTree pinyinTree(true);
	BigramLanguageModel lm;
	std::cout << model_file_path << std::endl;
	lm.Load(model_file_path);
	//lm.Load("chinese_pinyin.pkl");
	CLOG << "test_string_start: " << test_string_start << std::endl;
	for (int i = test_string_start; i < nArgs; i++)
	{
		test_split(pinyinTree, lm, vArgs[i], supportSplitBySingleIntitial);
	}
	return 0;
}
