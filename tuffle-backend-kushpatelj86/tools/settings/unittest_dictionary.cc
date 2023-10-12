#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "../../dictionary.h"
#include "../../gamestate.h"
#include "../../tuffle.h"
#include "../cppaudit/gtest_ext.h"

using ::testing::HasSubstr;
using ::testing::Not;

TEST(Dictionary, PublicMethodsPresent) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  dictionary.GetRandomTuffle();
  std::string guess = "tuffy";
  dictionary.IsValidGuess(guess);
}

TEST(Dictionary, IsValidGuessTrueForValidGuess) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  std::string guess = "hello";
  ASSERT_TRUE(dictionary.IsValidGuess(guess))
      << "The word hello should be a valid guess.";
}

TEST(Dictionary, IsValidGuessFalseForInvalidGuess) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  std::string guess = "zzzzz";
  ASSERT_FALSE(dictionary.IsValidGuess(guess))
      << "The word zzzzz should be a invalid guess.";
}

TEST(Dictionary, IsValidGuessFalseForTooShortGuess) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  std::string guess = "ab";
  ASSERT_FALSE(dictionary.IsValidGuess(guess))
      << "A guess is invalid if it's less than 5 characters long.";
  guess = "";
  ASSERT_FALSE(dictionary.IsValidGuess(guess))
      << "A guess is invalid if it's less than 5 characters long.";
}

TEST(Dictionary, IsValidGuessFalseForTooLongGuess) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  std::string guess = "ababab";
  ASSERT_FALSE(dictionary.IsValidGuess(guess))
      << "A guess is invalid if it's more than 5 characters long.";
  guess = "hellohello";
  ASSERT_FALSE(dictionary.IsValidGuess(guess))
      << "A guess is invalid if it's more than 5 characters long.";
}

TEST(Dictionary, GetRandomTuffleValidLength) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  std::string tuffle1 = dictionary.GetRandomTuffle();
  ASSERT_EQ(tuffle1.length(), 5)
      << "A word returned by GetRandomTuffle should be 5 characters long.";
}

TEST(Dictionary, GetRandomTuffleSubsequentWordsAreDifferent) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  std::string tuffle1 = dictionary.GetRandomTuffle();
  std::string tuffle2 = dictionary.GetRandomTuffle();
  std::string tuffle3 = dictionary.GetRandomTuffle();
  ASSERT_NE(tuffle1, tuffle2)
      << "Subsequent calls to GetRandomTuffle should return different words.";
  ASSERT_NE(tuffle2, tuffle3)
      << "Subsequent calls to GetRandomTuffle should return different words.";
}

TEST(Dictionary, RandomTuffleIsValidGuess) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  std::string tuffle = dictionary.GetRandomTuffle();
  ASSERT_TRUE(dictionary.IsValidGuess(tuffle))
      << "The word " << tuffle << " should be a valid guess.";
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  ::testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
