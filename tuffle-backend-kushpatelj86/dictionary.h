#include <string>
#include <unordered_set>
#include <vector>

#ifndef DICTIONARY_H
#define DICTIONARY_H
using namespace std;
class Dictionary {
 public:
  Dictionary(const std::string &valid_tuffles_filename,
             const std::string &valid_guesses_filename);

  // Returns true if the given word is a valid guess
  // according to this dictionary.
  bool IsValidGuess(const std::string &word) const;

  // Returns a random tuffle from the list of valid
  // tuffles (secret words) stored in this dictionary.
  const std::string &GetRandomTuffle() const;

 private:
  // valid_tuffles_ is a list of valid words from which the
  // secret tuffle may be chosen.
  std::vector<string> valid_tuffles_;
  // valid_guesses_ is a list of valid words that are considered
  // acceptable gueses a user can submit. We use a set here because
  // checking if a word is in a set is more time efficient than a vector.
  std::unordered_set<string> valid_guesses_;
};

#endif  // DICTIONARY_H