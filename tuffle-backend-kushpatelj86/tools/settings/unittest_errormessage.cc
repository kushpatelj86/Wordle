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

// Check that the "errorMessage" key exists in `game_state_json`
void CheckErrorMessage(json game_state_json) {
  ASSERT_TRUE(game_state_json.contains("errorMessage"))
      << "GameStateInJson should return a JSON object with the errorMessage "
         "set.";
  ASSERT_TRUE(game_state_json.at("errorMessage").is_string())
      << "GameStateInJson: errorMessage should be of type string.";
  std::string error = game_state_json.at("errorMessage");
  ASSERT_TRUE(error.length() > 0)
      << "GameStateInJson: errorMessage should be set to a message.";
}

// Check that there is no set error message.
void CheckEmptyOrNoErrorMessage(json game_state_json) {
  if (game_state_json.contains("errorMessage")) {
    ASSERT_TRUE(game_state_json.at("errorMessage").is_string())
        << "GameStateInJson: errorMessage should be of type string.";
    std::string error = game_state_json.at("errorMessage");
    ASSERT_TRUE(error.length() == 0) << "GameStateInJson: errorMessage should "
                                        "not be set when there is no error.";
  }
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

TEST(TuffleGame, GameStateInJsonHasErrorMessageWhenNotEnoughLetters) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('z');
  game.LetterKeyPressed('z');
  game.EnterKeyPressed();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckErrorMessage(game_state_json);
}

TEST(TuffleGame, GameStateInJsonHasErrorMessageWhenInvalidGuess) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('z');
  game.LetterKeyPressed('z');
  game.LetterKeyPressed('z');
  game.LetterKeyPressed('z');
  game.LetterKeyPressed('z');
  game.EnterKeyPressed();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckErrorMessage(game_state_json);
}

TEST(TuffleGame, NoErrorMessageWhenNewGame) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckEmptyOrNoErrorMessage(game_state_json);
}

TEST(TuffleGame, NoErrorMessageWhenNoError) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('i');
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckEmptyOrNoErrorMessage(game_state_json);
}

TEST(TuffleGame, ErrorMessageShouldBeEmptyWhenNoError) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.EnterKeyPressed();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckErrorMessage(game_state_json);
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckEmptyOrNoErrorMessage(game_state_json);
}

TEST(TuffleGame, NoErrorWhenSubmitThenErrorAfterTooFewLetters) {
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");
  TuffleGame game(dictionary);
  game.NewGame();
  game.LetterKeyPressed('h');
  game.LetterKeyPressed('e');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('l');
  game.LetterKeyPressed('o');
  game.EnterKeyPressed();
  json game_state_json = json::parse(game.GameStateInJson().dump());
  CheckEmptyOrNoErrorMessage(game_state_json);
  game.LetterKeyPressed('h');
  game.EnterKeyPressed();
  game_state_json = json::parse(game.GameStateInJson().dump());
  CheckErrorMessage(game_state_json);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::UnitTest::GetInstance()->listeners().Append(new SkipListener());
  return RUN_ALL_TESTS();
}
