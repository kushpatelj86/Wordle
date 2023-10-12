#include <string>
#include <vector>

#include "dictionary.h"
#include "gamestate.h"
#include "server_utils/crow_all.h"

#ifndef TUFFLE_H
#define TUFFLE_H

class TuffleGame {
 public:
  TuffleGame(Dictionary dict)
      : dictionary_(dict), game_state_(dict.GetRandomTuffle()) {}

  // Called by the Tuffle frontend when the user clicks "Next Tuffle".
  void NewGame();
  // Called by the Tuffle frontend when the user enters a letter.
  void LetterKeyPressed(char key);
  // Called by the Tuffle frontend when the user submits a guess.
  void EnterKeyPressed();
  // Called by the Tuffle frontend when the presses backspace.
  void DeleteKeyPressed();
  // GameStateInJson returns a JSON object representing the game state.
  // This is used to send the game state to the Tuffle Frontend in a readable
  // format.
  crow::json::wvalue GameStateInJson();

 private:
  GameState game_state_;
  Dictionary dictionary_;
};

#endif  // TUFFLE_H