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

// Check that the "answers" key exists in `game_state_json` with the given
// value.
void CheckAnswer(json game_state_json, const std::string& expected) {
  ASSERT_TRUE(game_state_json.contains("answer"))
      << "GameStateInJson should return a JSON object with the answer set.";
  ASSERT_TRUE(game_state_json.at("answer").is_string())
      << "GameStateInJson: answer should be of type string.";
  std::string answer = game_state_json.at("answer");
  ASSERT_EQ(answer, expected)
      << "The secret Tuffle answer should be " << expected;
}

// Check that the "boardColors" key exists in `game_state_json` with the
// expected size.
void CheckBoardColors(json game_state_json, int expected_size) {
  ASSERT_TRUE(game_state_json.contains("boardColors"))
      << "GameStateInJson should return a JSON object with the boardColors "
         "set.";
  ASSERT_TRUE(game_state_json.at("boardColors").is_array())
      << "GameStateInJson: boardColors should be a vector.";
  ASSERT_EQ(game_state_json.at("boardColors").size(), expected_size)
      << "GameStateInJson: boardColors should be a vector of size "
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

TEST(TuffleGame, GameStateInJsonHasAnswerWhenNewGame) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  ASSERT_TRUE(game_state_json.contains("answer"))
      << "GameStateInJson should return a JSON object with the answer set.";
  ASSERT_TRUE(game_state_json.at("answer").is_string())
      << "GameStateInJson: answer should be of type string.";
  std::string answer = game_state_json.at("answer");
  ASSERT_EQ(answer.length(), 5)
      << "The secret Tuffle answer should be 5 characters long.";
}

TEST(TuffleGame, GameStateInJsonHasBoardColorsWhenNewGame) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 0);
}

// Guessing 'drawn' should have no correct letters when the correct answer is
// 'light'
TEST(TuffleGame, BoardColorsAllIncorrectLettersInGuess) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterKeyPressed('d');
  game.LetterKeyPressed('r');
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('w');
  game.LetterKeyPressed('n');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BBBBB")
      << "Correct Tuffle: light\nGuess made: drawn\nboardColors should be "
         "BBBBB when the guess has no correct letters.";
}

// Guessing 'plays' should have a yellow letter when the correct answer is
// 'light'
TEST(TuffleGame, BoardColorsCorrectLetterWrongPlace) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterKeyPressed('p');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('y');
  game.LetterKeyPressed('s');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BYBBB")
      << "Correct Tuffle: light\nGuess made: plays\nboardColors should be "
         "BYBBB when the guess has a correct letter in the wrong position.";
}

// Guessing 'gloss' should have 2 yellow letters when the correct answer is
// 'light'
TEST(TuffleGame, BoardColorsCorrectLettersWrongPlace) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game.LetterKeyPressed('s');
  game.LetterKeyPressed('s');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "YYBBB")
      << "Correct Tuffle: light\nGuess made: gloss\nboardColors should be "
         "YYBBB when the guess has two correct letters in the wrong position.";
}

// Guessing 'light' should have all green letters when the correct answer is
// 'light'
TEST(TuffleGame, BoardColorsAllCorrectLettersInGuess) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('i');
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('t');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "GGGGG")
      << "Correct Tuffle: light\nGuess made: light\nboardColors should be "
         "GGGGG when the guess is correct.";
}

TEST(TuffleGame, BoardColorsMultipleGuesses) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckAnswer(game_state_json, "light");
  game.LetterKeyPressed('d');
  game.LetterKeyPressed('r');
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('w');
  game.LetterKeyPressed('n');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BBBBB")
      << "Correct Tuffle: light\nGuess made: drawn\nboardColors should be "
         "BBBBB when the guess has no correct letters.";
  game.LetterKeyPressed('p');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('y');
  game.LetterKeyPressed('s');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 2);
  std::string guess2 = game_state_json.at("boardColors").at(1);
  ASSERT_EQ(guess2, "BYBBB")
      << "Correct Tuffle: light\nGuess made: plays\nboardColors should be "
         "BYBBB when the guess has a correct letter in the wrong position.";
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game.LetterKeyPressed('s');
  game.LetterKeyPressed('s');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 3);
  std::string guess3 = game_state_json.at("boardColors").at(2);
  ASSERT_EQ(guess3, "YYBBB")
      << "Correct Tuffle: light\nGuess made: gloss\nboardColors should be "
         "YYBBB when the guess has two correct letters in the wrong position.";
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('i');
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('t');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 4);
  std::string guess4 = game_state_json.at("boardColors").at(3);
  ASSERT_EQ(guess4, "GGGGG")
      << "Correct Tuffle: light\nGuess made: light\nboardColors should be "
         "GGGGG when the guess is correct.";
}

TEST(TuffleGame, BoardColorsAllIncorrectLettersInGuessForTuffy) {
  Dictionary dictionary("tools/settings/data/tuffy.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckAnswer(game_state_json, "tuffy");
  game.LetterKeyPressed('d');
  game.LetterKeyPressed('r');
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('w');
  game.LetterKeyPressed('n');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BBBBB")
      << "Correct Tuffle: tuffy\nGuess made: drawn\nboardColors should be "
         "BBBBB when the guess has no correct letters.";
}

TEST(TuffleGame,
     BoardColorsDuplicateLettersInGuessShouldPrioritizeGreenLetters) {
  Dictionary dictionary("tools/settings/data/tuffy.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckAnswer(game_state_json, "tuffy");
  game.LetterKeyPressed('y');
  game.LetterKeyPressed('u');
  game.LetterKeyPressed('m');
  game.LetterKeyPressed('m');
  game.LetterKeyPressed('y');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BGBBG")
      << "Correct Tuffle: tuffy\nGuess made: yummy\nboardColors should be "
         "BGBBG when the guess has no correct letters.";
}

TEST(TuffleGame, BoardColorsDuplicateFLettersInGuessForTuffy) {
  Dictionary dictionary("tools/settings/data/tuffy.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckAnswer(game_state_json, "tuffy");
  game.LetterKeyPressed('s');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('a');
  game.LetterKeyPressed('f');
  game.LetterKeyPressed('f');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckBoardColors(game_state_json, 1);
  std::string guess1 = game_state_json.at("boardColors").at(0);
  ASSERT_EQ(guess1, "BYBGY")
      << "Correct Tuffle: tuffy\nGuess made: staff\nboardColors should be "
         "BYBGY when the guess has no correct letters.";
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
