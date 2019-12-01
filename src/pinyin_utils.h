#pragma once
#ifndef __PINYIN_UITILS_H__
#define __PINYIN_UITILS_H__
#include <list>
#include <iostream>
#include <string>

#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

#ifndef __inout
#define __inout
#endif

const int kMaxPinyinChars = 26;
typedef std::list<std::string> StringList;

inline std::ostream &operator<<(__in std::ostream &os, __in const StringList &slist)
{
	for (auto &s : slist)
	{
		std::cout << s << " ";
	}
	return os;
}

class LogStream : public std::streambuf, public std::ostream{
public:
  class Voidify {
  public:
      Voidify() { }
      // This has to be an operator with a precedence lower than << but
      // higher than ?:
      void operator&(std::ostream&) { }
  };

    std::ostream& out_stream_;
    LogStream(std::ostream& stream = std::cerr) : enabled_(false), std::ostream(this), out_stream_(stream){}
    template<typename T> 
    LogStream& operator<< (const T& data) 
    {
      if (Enabled()) out_stream_ << data;
        return *this;
    }

    inline LogStream& operator<<(LogStream& (*m)(LogStream&)) {
        return m(*this);
    }

    LogStream& operator<< (std::ostream& (*pf)(std::ostream&)) 
    {
      if (Enabled()) out_stream_ << pf;
        return *this;
    }

    bool Enabled() const{
      return enabled_;
    }

    void Enable(bool v){
      enabled_ = v;
    }

private:
  bool enabled_;
};

#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void) 0 : ::LogStream::Voidify() & (stream)

// class log
#define CLOG LAZY_STREAM(class_xx_log__, class_xx_log__.Enabled())
#define DEFINE_CLOG LogStream class_xx_log__;
#define SET_CLOG(enabled) class_xx_log__.Enable(enabled)

const char *kInitialsTable[] = {
		"b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n",
		"p", "q", "r", "s", "t", "w", "x", "y", "z", "sh", "ch", "zh"};

bool IsInitials(const std::string &s)
{
	for (size_t i = 0; i < sizeof(kInitialsTable) / sizeof(const char *); i++)
	{
		if (s == kInitialsTable[i])
		{
			return true;
		}
	}
	return false;
}

const char *kPinyinTable[] = {
		"a", "ai", "an", "ang", "ao",
		"ba", "bai", "ban", "bang", "bao", "bei", "ben", "beng", "bi", "bian", "biao", "bie", "bin", "bing", "bo", "bu",
		"ca", "cai", "can", "cang", "cao", "ce", "cen", "ceng",
		"cha", "chai", "chan", "chang", "chao", "che", "chen", "cheng", "chi", "chong", "chou", "chu", "chuai", "chuan", "chuang", "chui", "chun", "chuo",
		"ci", "cong", "cou", "cu", "cuan", "cui", "cun", "cuo",
		"da", "dai", "dan", "dang", "dao", "de", "dei", "den", "deng", "di", "dia", "dian", "diao", "die", "ding", "diu", "dong", "dou", "du", "duan", "dui", "dun", "duo",
		"e", "ei", "en", "eng", "er",
		"fa", "fan", "fang", "fei", "fen", "feng", "fo", "fou", "fu",
		"ga", "gai", "gan", "gang", "gao", "ge", "gei", "gen", "geng", "gong", "gou", "gu", "gua", "guai", "guan", "guang", "gui", "gun", "guo",
		"ha", "hai", "han", "hang", "hao", "he", "hei", "hen", "heng", "hong", "hou", "hu", "hua", "huai", "huan", "huang", "hui", "hun", "huo",
		"ji", "jia", "jian", "jiang", "jiao", "jie", "jin", "jing", "jiong", "jiu", "ju", "juan", "jue", "jun",
		"ka", "kai", "kan", "kang", "kao", "ke", "kei", "ken", "keng", "kong", "kou", "ku", "kua", "kuai", "kuan", "kuang", "kui", "kun", "kuo",
		"la", "lai", "lan", "lang", "lao", "le", "lei", "leng", "li", "lia", "lian", "liang", "liao", "lie", "lin", "ling", "liu", "lo", "long", "lou", "lu", "luan", "lun", "luo", "lv", "lve",
		"ma", "mai", "man", "mang", "mao", "me", "mei", "men", "meng", "mi", "mian", "miao", "mie", "min", "ming", "miu", "mo", "mou", "mu",
		"na", "nai", "nan", "nang", "nao", "ne", "nei", "nen", "neng", "ni", "nian", "niang", "niao", "nie", "nin", "ning", "niu", "nong", "nou", "nu", "nuan", "nun", "nuo", "nv", "nve",
		"o", "ou",
		"pa", "pai", "pan", "pang", "pao", "pei", "pen", "peng", "pi", "pian", "piao", "pie", "pin", "ping", "po", "pou", "pu",
		"qi", "qia", "qian", "qiang", "qiao", "qie", "qin", "qing", "qiong", "qiu", "qu", "quan", "que", "qun",
		"ran", "rang", "rao", "re", "ren", "reng", "ri", "rong", "rou", "ru", "ruan", "rui", "run", "ruo",
		"sa", "sai", "san", "sang", "sao", "se", "sen", "seng",
		"sha", "shai", "shan", "shang", "shao", "she", "shei", "shen", "sheng", "shi", "shou", "shu", "shua", "shuai", "shuan", "shuang", "shui", "shun", "shuo",
		"si", "song", "sou", "su", "suan", "sui", "sun", "suo",
		"ta", "tai", "tan", "tang", "tao", "te", "teng", "ti", "tian", "tiao", "tie", "ting", "tong", "tou", "tu", "tuan", "tui", "tun", "tuo",
		"wa", "wai", "wan", "wang", "wei", "wen", "weng", "wo", "wu",
		"xi", "xia", "xian", "xiang", "xiao", "xie", "xin", "xing", "xiong", "xiu", "xu", "xuan", "xue", "xun",
		"ya", "yan", "yang", "yao", "ye", "yi", "yin", "ying", "yo", "yong", "you", "yu", "yuan", "yue", "yun",
		"za", "zai", "zan", "zang", "zao", "ze", "zei", "zen", "zeng",
		"zha", "zhai", "zhan", "zhang", "zhao", "zhe", "zhei", "zhen", "zheng", "zhi", "zhong", "zhou", "zhu", "zhua", "zhuai", "zhuan", "zhuang", "zhui", "zhun", "zhuo",
		"zi", "zong", "zou", "zu", "zuan", "zui", "zun", "zuo"};

inline bool IsValidPinyinChar(char c)
{
	return c >= 'a' and c <= 'z';
}

inline bool IsWowelChar(char c)
{
	return c == 'a' || c == 'e' || c == 'i' || c == 'u' || c == 'o';
}

inline bool IsInvalidPinyin(const std::string &pinyin)
{
	if (pinyin.length() == 0)
	{
		return false;
	}

	for (size_t i = 0; i < pinyin.length(); i++)
	{
		char c = pinyin[i];
		if (!IsValidPinyinChar(c))
		{
			return false;
		}
	}
	return true;
}
#endif // __PINYIN_UITILS_H__