#include "tuffle.h"

#include <cstddef>
#include <string>

using namespace std;
// ========================= YOUR CODE HERE =========================
// This implementation file is where you should implement the member
// functions declared in the header, only if you didn't implement
// them inline in the header.
//
// Remember to specify the name of the class with :: in this format:
//     <return type> MyClassName::MyFunction() {
//        ...
//     }
void TuffleGame::NewGame() 
{
  game_state_ = GameState(dictionary_.GetRandomTuffle());
}

void TuffleGame::LetterKeyPressed(char key) {
      
   game_state_.AddLetter(key);

}

void TuffleGame::EnterKeyPressed() {
   
    std::string answer = game_state_.GetGuess();
    game_state_.ComparingTwoWords(dictionary_.IsValidGuess(answer));
 
   
    
  }
    
  



  
    
  
  
  


void TuffleGame::DeleteKeyPressed() {
 game_state_.Delete();
}

// to tell the compiler that each function belongs to the TuffleGame class.
// ===================================================================

crow::json::wvalue TuffleGame::GameStateInJson() {
  // The JSON object to return to the Tuffle Frontend.
  crow::json::wvalue game_state_json({});

  // ===================== YOUR CODE HERE =====================
  // Fill the game_state_json with the data expected by the
  // Tuffle frontend. The frontend expects the following keys:
  //   1. "answer"
  //   2. "boardColors"
  //   3. "guessedWords"
  //   4. "gameStatus"
  //   5. "errorMessage"
  //   6. [OPTIONAL] "letterColors"
  // See the "JSON Response" section of tinyurl.com/cpsc121-f22-tuffle
  //
  // You can set the key in the JSON to a value like so:
  //             game_state_json[<key>] = <value>
  //
  // See below for an example to set the "answer" key:
  game_state_json["answer"] = game_state_.GetSecretTuffle();  // Replace this!
  game_state_json["boardColors"] = game_state_.GetBoardColors();
  game_state_json["guessedWords"] = game_state_.GetGuessedWords();
  game_state_json["gameStatus"] = game_state_.GetGameStatus();  // Replace this!
  game_state_json["errorMessage"] = game_state_.GetMessage();   // Replace this!

  // ==========================================================
  return game_state_json;
}