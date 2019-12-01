#pragma once
#ifndef __PINYIN_TIRE_TREE_H__
#define __PINYIN_TIRE_TREE_H__
#include "pinyin_utils.h"
#include <fstream>

class TireNode
{
public:
  enum class LeafType
  {
    None = 0,
    Pinyin = 1,
    Initial = 2,
  };

  ~TireNode()
  {
    DeleteChildren();
  }

  void Add(const std::string &pinyin, size_t start_index, LeafType leaf_type)
  {
    if (start_index == pinyin.length())
    {
      this->leaf_type_ = leaf_type;
      return;
    }

    char c = pinyin[start_index];
    TireNode *&child = children_[c - 'a'];
    if (child == nullptr)
    {
      child = new TireNode();
      child->value_ = c;
    }

    child->Add(pinyin, start_index + 1, leaf_type);
  }

  TireNode *GetAt(char c) const
  {
    return children_[c - 'a'];
  }

  bool IsEnd() const
  {
    return leaf_type_ != LeafType::None;
  }

  char value() const
  {
    return value_;
  }

  LeafType leafType() const
  {
    return leaf_type_;
  }

private:
  void DeleteChildren()
  {
    for (int i = 0; i < kMaxPinyinChars; i++)
    {
      if (children_[i] != nullptr)
      {
        delete children_[i];
        children_[i] = nullptr;
      }
    }
  }

  LeafType leaf_type_ = LeafType::None;
  char value_ = '#';
  TireNode *children_[kMaxPinyinChars] = {};
};

inline const char *GetLeafTypeName(__In const TireNode::LeafType &leaf_type)
{
  switch (leaf_type)
  {
  case TireNode::LeafType::None:
    return "None";
  case TireNode::LeafType::Pinyin:
    return "Pinyin";
  case TireNode::LeafType::Initial:
    return "Initial";
  default:
    return "Default";
  }
}

inline std::ostream &operator<<(__In std::ostream &os, __In const TireNode::LeafType &leaf_type)
{
  os << GetLeafTypeName(leaf_type);
  return os;
}

class PinyinTireTree
{
public:
  PinyinTireTree(bool supportSplitBySingleIntitial = true)
  {
    LoadFromBuiltIn(supportSplitBySingleIntitial);
  }

  void LoadFromBuiltIn(bool supportSplitBySingleIntitial)
  {
    for (size_t i = 0; i < sizeof(kPinyinTable) / sizeof(const char *); i++)
    {
      Add(kPinyinTable[i]);
    }

    if (supportSplitBySingleIntitial)
    {
      for (size_t i = 0; i < sizeof(kInitialsTable) / sizeof(const char *); i++)
      {
        Add(kInitialsTable[i], TireNode::LeafType::Initial);
      }
    }
  }

  bool IsPinyin(const std::string &s)
  {
    TireNode *current = &root;
    for (size_t i = 0; i < s.length(); i++)
    {
      auto c = s[i];
      if (!IsValidPinyinChar(c))
      {
        return false;
      }

      if (current->GetAt(c) != nullptr)
      {
        current = current->GetAt(c);
      }
      else
      {
        return false;
      }
    }

    return current->IsEnd();
  }

  const TireNode &Root() const
  {
    return root;
  }

private:
  void Add(const std::string &pinyin, TireNode::LeafType leafType = TireNode::LeafType::Pinyin)
  {
    if (!IsInvalidPinyin(pinyin))
    {
      return;
    }
    root.Add(pinyin, 0, leafType);
  }

  TireNode root;
};

class PinyinSpiltResult
{
public:
  static PinyinSpiltResult *Create(const char *str, const TireNode *root, bool support_initials)
  {
    if (!str)
    {
      str = "";
    }

    auto split_result = new PinyinSpiltResult(str, root);
    if (!split_result)
    {
      return split_result;
    }

    split_result->support_initials_ = support_initials;

    split_result->Split();

    return split_result;
  }

  static void Destroy(PinyinSpiltResult *split_result)
  {
    if (split_result)
    {
      delete split_result;
    }
  }

  bool GetTopN(__Out std::list<StringList> &topN, int N = -1)
  {
    if (N < 0)
    {
      topN = return_result;
    }

    // Todo
    topN = return_result;
    return true;
  }

  const char *ErrorText() const
  {
    return error_text_.c_str();
  }

private:
  PinyinSpiltResult(__In const std::string &text, const TireNode *root)
      : s_(text), root_(root)
  {
    first_vowel_pos_ = 0;
    last_vowel_pos_ = s_.length();
    //SET_CLOG(true);
  }

  ~PinyinSpiltResult()
  {
  }

  bool ListMatchRule(const StringList &result)
  {
    return true;
  }

  bool IsTokenEnd(__In const TireNode *current, __Inout size_t &pinyin_start_index, size_t current_index) const
  {
    TireNode::LeafType current_leaf_type = current->leafType();

    switch (current_leaf_type)
    {
    case TireNode::LeafType::None:
      return false;
    case TireNode::LeafType::Pinyin:
      return true;
    case TireNode::LeafType::Initial:
      if (support_initials_)
      {
      #ifdef SUPPORT_INITIAL_SLOW_METHOD
        return true;
      #else
        return (pinyin_start_index < first_vowel_pos_ || pinyin_start_index > last_vowel_pos_);
      #endif
      }
      else
      {
        return false;
      }
    default:
      return false;
    }
  }

  bool BeginSentence()
  {
    CLOG << "BeginSentence" << std::endl;
    StringList stackable_result;
    size_t pinyin_start_index = 0;
    BeginToken(pinyin_start_index, stackable_result, return_result);
    return return_result.size() > 0;
  }

  void EndSentence(__Inout StringList &result)
  {
    CLOG << "EndSentence: " << result << std::endl;
    //std::cout << "Find One: " << result << std::endl;
    if (ListMatchRule(result))
    {
      return_result.push_back(result);
    }
  }

  void BeginToken(
      __In size_t pinyin_start_index,
      __Inout StringList &result,
      __Out std::list<StringList> &return_result)
  {
    // get the
    CLOG << "BeginToken: " << s_[pinyin_start_index] << std::endl;
    TireNode *begin_node = root_->GetAt(s_[pinyin_start_index]);
    if (begin_node)
    {
      ProcessToken(pinyin_start_index, 1, begin_node, result, return_result);
    }
  }

  void EndToken(
      __In size_t pinyin_start_index,
      size_t current_length,
      __Inout StringList &result)
  {
    CLOG << "EndToken" << std::endl;
    std::string pinpyin = s_.substr(pinyin_start_index, current_length);
    result.push_back(pinpyin);

    pinyin_start_index += current_length;
    if (pinyin_start_index == s_.length())
    {
      EndSentence(result);
    }
    else
    {
      BeginToken(pinyin_start_index, result, return_result);
    }
    result.pop_back();
  }

  void ProcessToken(
      __In size_t pinyin_start_index,
      size_t current_length,
      __In const TireNode *current,
      __Inout StringList &result,
      __Out std::list<StringList> &return_result)
  {
    const std::string &s = s_;
    if (current == nullptr)
    {
      return;
    }

    CLOG << "ProcessToken: [" << result << "]|"
              << pinyin_start_index << " " << s[pinyin_start_index] << " "
              << current_length << " " << s[pinyin_start_index + current_length - 1] << " "
              << "curent value: " << current->value() << " "
              << "curent type: " << current->leafType() << " "
              << std::endl;

    if (IsTokenEnd(current, pinyin_start_index, current_length))
    {
      EndToken(pinyin_start_index, current_length, result);
    }

    if (pinyin_start_index + current_length >= s.length())
    {
      return;
    }

    TireNode *matched_child = current->GetAt(s[pinyin_start_index + current_length]);
    if (matched_child == nullptr)
    {
      return;
    }
    else
    {
      ProcessToken(pinyin_start_index, current_length + 1, matched_child, result, return_result);
    }
  }

  bool PrepareScan(__In const std::string &s)
  {
    /* first, scan the string, find:
      1. if have any invalid char, then return
      2. get the first and last vowel position
    */
    if (s.length() == 0)
    {
      return false;
    }

    for (size_t i = 0; i < s.length(); i++)
    {
      char c = s[i];
      if (!IsValidPinyinChar(c))
      {
        return false;
      }

      if (IsWowelChar(c))
      {
        if (first_vowel_pos_ == 0)
        {
          first_vowel_pos_ = i;
        }

        last_vowel_pos_ = i;
      }
    }
    return true;
  }

  bool Split()
  {
    if (!PrepareScan(s_))
    {
      return false;
    }

    return BeginSentence();
  }

private:
  std::string error_text_;
  const TireNode *root_ = nullptr;
  const std::string s_;
  bool support_initials_ = true;
  size_t first_vowel_pos_ = 0;
  size_t last_vowel_pos_ = 0;
  std::list<StringList> return_result;
  DEFINE_CLOG;
};
#endif