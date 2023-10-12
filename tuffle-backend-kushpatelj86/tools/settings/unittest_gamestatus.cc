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

// Check that the "gameStatus" key exists in `game_state_json` with the given
// value.
void CheckGameStatus(json game_state_json, const std::string &expected,
                     const std::string &error_message) {
  ASSERT_TRUE(game_state_json.contains("gameStatus"))
      << "GameStateInJson should return a JSON object with the gameStatus set.";
  ASSERT_TRUE(game_state_json.at("gameStatus").is_string())
      << "GameStateInJson: gameStatus should be of type string.";
  std::string status = game_state_json.at("gameStatus");
  ASSERT_EQ(status, expected)
      << "The gameStatus should be " << expected << " " << error_message;
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

TEST(TuffleGame, GameStateInJsonHasActiveGameStatusWhenNewGame) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGameStatus(game_state_json, "active", "when a new game has started.");
}

TEST(TuffleGame, GameStateInJsonHasWinGameStatusWhenVictory) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('i');
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('t');
  game.EnterKeyPressed();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGameStatus(game_state_json, "win",
                  "when the user correctly guesses the secret Tuffle.");
}

TEST(TuffleGame, GameStateInJsonHasLoseGameStatusWhenSixIncorrectGuesses) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  for (int i = 0; i < 6; i++) {
    game.LetterKeyPressed('h');
    game.LetterKeyPressed('e');
    game.LetterKeyPressed('l');
    game.LetterKeyPressed('l');
    game.LetterKeyPressed('o');
    game.EnterKeyPressed();
  }
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGameStatus(game_state_json, "lose",
                  "when a user guesses incorrectly six times.");
}

TEST(TuffleGame, NewGameAfterWinShouldResetGameStatusToActive) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('i');
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('t');
  game.EnterKeyPressed();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGameStatus(game_state_json, "win",
                  "when the user correctly guesses the secret Tuffle.");
  game.NewGame();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGameStatus(game_state_json, "active", "when a new game has started.");
}

TEST(TuffleGame, NewGameAfterLossShouldResetGameStatusToActive) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  for (int i = 0; i < 6; i++) {
    game.LetterKeyPressed('h');
    game.LetterKeyPressed('e');
    game.LetterKeyPressed('l');
    game.LetterKeyPressed('l');
    game.LetterKeyPressed('o');
    game.EnterKeyPressed();
  }
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGameStatus(game_state_json, "lose",
                  "when a user guesses incorrectly six times.");
  game.NewGame();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGameStatus(game_state_json, "active", "when a new game has started.");
}

TEST(TuffleGame,
     GameStateInJsonHasWinGameStatusWhenVictoryExtraCharactersPressed) {
  Dictionary dictionary("tools/settings/data/light.txt",
                        "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('i');
  game.LetterKeyPressed('g');
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('t');
  game.LetterKeyPressed('t');
  game.EnterKeyPressed();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckGameStatus(game_state_json, "win",
                  "when the user correctly guesses the secret Tuffle.");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
