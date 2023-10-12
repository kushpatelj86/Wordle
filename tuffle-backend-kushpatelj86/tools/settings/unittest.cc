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

TEST(TuffleGame, GameStateInJsonHasExpectedKeys) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  crow::json::wvalue game_state = game.GameStateInJson();
  std::vector<std::string> keys = game_state.keys();
  ASSERT_TRUE(std::find(keys.begin(), keys.end(), "answer") != keys.end())
      << "GameStateInJson should return a JSON object with the answer set.";
  ASSERT_TRUE(std::find(keys.begin(), keys.end(), "boardColors") != keys.end())
      << "GameStateInJson should return a JSON object with the boardColors "
         "set.";
  ASSERT_TRUE(std::find(keys.begin(), keys.end(), "guessedWords") != keys.end())
      << "GameStateInJson should return a JSON object with the guessedWords "
         "set.";
  ASSERT_TRUE(std::find(keys.begin(), keys.end(), "gameStatus") != keys.end())
      << "GameStateInJson should return a JSON object with the gameStatus set.";
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

TEST(TuffleGame, GameStateInJsonHasGuessedWordsWhenNewGame) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  ASSERT_TRUE(game_state_json.contains("guessedWords"))
      << "GameStateInJson should return a JSON object with the guessedWords "
         "set.";
  ASSERT_TRUE(game_state_json.at("guessedWords").is_array())
      << "GameStateInJson: guessedWords should be a vector.";
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
