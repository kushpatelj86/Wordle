#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "../../dictionary.h"
#include "../../gamestate.h"
#include "../../server_utils/crow_all.h"
#include "../../tuffle.h"
#include "../cppaudit/gtest_ext.h"

using ::testing::HasSubstr;
using ::testing::Not;
using json = nlohmann::json;

// Check that the "guessedWords" key exists in `game_state_json` with the
// expected size.
void CheckGuessedWords(json game_state_json, int expected_size) {
  ASSERT_TRUE(game_state_json.contains("guessedWords"))
      << "GameStateInJson should return a JSON object with the guessedWords "
         "set.";
  ASSERT_TRUE(game_state_json.at("guessedWords").is_array())
      << "GameStateInJson: guessedWords should be a vector.";
  ASSERT_EQ(game_state_json.at("guessedWords").size(), expected_size)
      << "GameStateInJson: guessedWords should be a vector of size "
      << expected_size;
}

TEST(TuffleGame, PublicMethodsPresent) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('a');
  game.EnterKeyPressed();
  game.DeleteKeyPressed();
  game.GameStateInJson();
}

TEST(TuffleGame, GameStateInJsonHasGuessedWordsWhenNewGame) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 0);
}

TEST(TuffleGame, GameStateInJsonHasGuessedWordsWhenOneLetterTyped) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('d');
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
}

TEST(TuffleGame, GameStateInJsonHasGuessedWordsWhenOneWordSubmitted) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game.EnterKeyPressed();
  game.LetterKeyPressed('h');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 2);
}

TEST(TuffleGame, GameStateInJsonHasGuessedWordsWhenMultipleWordsSubmitted) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  game.EnterKeyPressed();
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 2);
  game.EnterKeyPressed();
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 3);
}

TEST(TuffleGame, GameStateInJsonHasGuessedWordsAfterNewGameReset) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  game.EnterKeyPressed();
  game.LetterKeyPressed('h');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 2);
  game.NewGame();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 0);
}

TEST(TuffleGame, PressingMoreThanFiveCharactersDoesNotChangeState) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  game.LetterKeyPressed('s');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  game.LetterKeyPressed('s');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
}

TEST(TuffleGame, GuessedWordsStoresPartiallyTypedWord) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HE")
      << "After typing 'h' and 'e', guessedWords should record that the word "
         "HE is a partially typed word.";
}

TEST(TuffleGame, GuessedWordsStoresOneWord) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HELLO")
      << "guessedWords should record that the word HELLO was typed as a guess.";
}

TEST(TuffleGame, PressingMoreThanFiveCharactersDoesNotChangeGuessedWord) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game.LetterKeyPressed('s');
  game.LetterKeyPressed('s');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HELLO")
      << "After typing 'helloss', the extra 'ss' is ignored and guessedWords "
         "should record the word HELLO.";
}

TEST(TuffleGame, GuessedWordsStoresMultipleSubmittedWords) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HELLO")
      << "guessedWords should record that the word HELLO was typed as a guess.";
  game.EnterKeyPressed();
  game.LetterKeyPressed('n');
  game.LetterKeyPressed('i');
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('t');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 2);
  std::string guess2 = game_state_json.at("guessedWords").at(1);
  std::for_each(guess2.begin(), guess2.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess2, "NIGHT")
      << "guessedWords should record that the word NIGHT was typed as a guess.";
}

TEST(TuffleGame, DeletingCharactersChangesGuessedWord) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game.DeleteKeyPressed();
  game.DeleteKeyPressed();
  game.DeleteKeyPressed();
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('r');
  game.LetterKeyPressed('t');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HEART")
      << "After typing 'hello' and backspacing 3 times and typing 'art', "
         "guessedWords should record the word HEART.";
}

TEST(TuffleGame,
     DeletingCharactersAfterMultipleSubmittedWordsChangesGuessedWords) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game.DeleteKeyPressed();
  game.DeleteKeyPressed();
  game.DeleteKeyPressed();
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('r');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('s');
  game.LetterKeyPressed('y');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HEART")
      << "After typing 'hello' and backspacing 3 times and typing 'artsy', "
         "guessedWords should record the word HEART.";
  game.EnterKeyPressed();
  game.LetterKeyPressed('n');
  game.LetterKeyPressed('i');
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('t');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 2);
  std::string guess2 = game_state_json.at("guessedWords").at(1);
  std::for_each(guess2.begin(), guess2.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess2, "NIGHT")
      << "guessedWords should record that the word NIGHT was typed as a guess.";
  game.EnterKeyPressed();
  game.LetterKeyPressed('t');
  game.DeleteKeyPressed();
  game.LetterKeyPressed('c');
  game.LetterKeyPressed('p');
  game.LetterKeyPressed('p');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 3);
  std::string guess3 = game_state_json.at("guessedWords").at(2);
  std::for_each(guess3.begin(), guess3.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess3, "CPP")
      << "After typing 't', backspace, then 'cpp', guessedWords should record "
         "that the word CPP was typed as a guess.";
}

TEST(TuffleGame, SubmittingInvalidWordDoesNotChangeGuessedWords) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.EnterKeyPressed();
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game.DeleteKeyPressed();
  game.DeleteKeyPressed();
  game.DeleteKeyPressed();
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('r');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('s');
  game.LetterKeyPressed('y');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 1);
  std::string guess1 = game_state_json.at("guessedWords").at(0);
  std::for_each(guess1.begin(), guess1.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess1, "HEART")
      << "After typing 'hello' and backspacing 3 times and typing 'artsy', "
         "guessedWords should record the word HEART.";
  game.EnterKeyPressed();
  game.LetterKeyPressed('n');
  game.LetterKeyPressed('i');
  game.EnterKeyPressed();
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('h');
  game.EnterKeyPressed();
  game.LetterKeyPressed('t');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 2);
  std::string guess2 = game_state_json.at("guessedWords").at(1);
  std::for_each(guess2.begin(), guess2.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess2, "NIGHT")
      << "guessedWords should record that the word NIGHT was typed as a guess.";
  game.EnterKeyPressed();
  game.LetterKeyPressed('t');
  game.DeleteKeyPressed();
  game.LetterKeyPressed('c');
  game.LetterKeyPressed('p');
  game.EnterKeyPressed();
  game.LetterKeyPressed('p');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGuessedWords(game_state_json, 3);
  std::string guess3 = game_state_json.at("guessedWords").at(2);
  std::for_each(guess3.begin(), guess3.end(),
                [](char &c) { c = ::toupper(c); });
  ASSERT_EQ(guess3, "CPP")
      << "After typing 't', backspace, then 'cpp', guessedWords should record "
         "that the word CPP was typed as a guess.";
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
