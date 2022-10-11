#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <vector>
using namespace std;

typedef string_view Prefix;

struct PrefixCmp {
  bool operator()(const Prefix& prefix1, const Prefix& prefix2) const {
    if (prefix1.length() < prefix2.length())
      return true;
    if (prefix1.length() > prefix2.length())
      return false;
    return prefix1 < prefix2;
  }
};

struct PrefixInfo {
  string continuations;
  int score;
  int size;
  bool is_word;
};

typedef map<Prefix,PrefixInfo,PrefixCmp> PrefixTree;

const vector<string>& get_dictionary() {
  static vector<string> dictionary;
  if (dictionary.empty()) {
    ifstream in("es");
    //ifstream in("es");
    if (not in) {
      cerr << "Couldn't load dictionary\n";
      exit(1);
    }
    string word;
    while (in >> word) {
      if (word.length() > 3)
        dictionary.push_back(move(word));
    }
  }
  return dictionary;
}

bool add_word(PrefixTree& tree, const Prefix& word) {
  auto& info = tree[word];
  bool new_word = !info.is_word;
  info.is_word = true;
  if (new_word)
    info.size++;
  return new_word;
}

PrefixTree compute_prefix_tree(const vector<string>& words) {
  PrefixTree prefix_tree;
  for (const auto& word : words) {
    Prefix prefix(word);
    bool new_word = add_word(prefix_tree, prefix);
    while (!prefix.empty()) {
      char last = prefix.back();
      prefix.remove_suffix(1);
      auto& info = prefix_tree[prefix];
      if (new_word)
        info.size++;
      if (info.continuations.find(last) == string::npos)
        info.continuations.push_back(last);
    }
  }
  return prefix_tree;
}

void backup_scores(PrefixTree& tree) {
  for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
    auto& info = it->second;
    int score = 0;
    for (char next_c : info.continuations) {
      string nxt(it->first.begin(), it->first.end());
      nxt += next_c;
      const auto& info_child = tree.find(Prefix(nxt))->second;
      int tent_score = info_child.score + 1;
      if (score%2 == 0 && tent_score%2 == 1)
        score = tent_score;
      else if (score%2 == 0 && tent_score%2 == 0)
        score = max(score, tent_score);
      else if (score%2 == 1 && tent_score%2 == 1)
        score = min(score, tent_score);
    }
    info.score = score;
  }
}

constexpr char k_challenge = '0';
constexpr char k_defeat = '1';

char play(const PrefixTree& tree, const string& acc) {
  auto it = tree.find(Prefix(acc));
  if (it == tree.end())
    return k_challenge;
  const auto& info = it->second;
  char best_play = k_defeat;
  for (char next_c : info.continuations) {
    string nxt = acc + next_c;
    const auto& info_child = tree.find(Prefix(nxt))->second;
    int tent_score = info_child.score + 1;
    if (tent_score == info.score)
      best_play = next_c;
  }
  return best_play;
}

string challenge(const PrefixTree& tree, string acc) {
  char c;
  while ((c=play(tree,acc))!=k_defeat) {
    acc += c;
  }
  return acc;
}

constexpr int cpu_player = 0;
constexpr int human_player = 0;

int main() {
  const auto& words = get_dictionary();
  PrefixTree prefix_tree = compute_prefix_tree(words);
  backup_scores(prefix_tree);

  cout << "First player? (0: computer, 1: human)" << endl;
  int next_player;
  cin >> next_player;

  string word;
  while (true) {
    char last_play;
    if (next_player == cpu_player)
      last_play = play(prefix_tree, word);
    else
      cin >> last_play;
    next_player = 1 - next_player;
    if (last_play == k_challenge) {
      if (next_player == cpu_player) {
        cout << "Proposed word: "
             << challenge(prefix_tree, word) << endl
             << "Computer wins!";
      }
      else {
        cout << "No known word starting with " << word
             << ". Computer wins!" << endl;
      }
      break;
    }
    else if (last_play == k_defeat) {
      if (next_player == cpu_player)
        cout << "Computer wins!" << endl;
      else
        cout << "Human wins!" << endl;
      break;
    }
    else {
      word += last_play;
      cout << "Word so far: " << word << endl;
    }
  }
}
