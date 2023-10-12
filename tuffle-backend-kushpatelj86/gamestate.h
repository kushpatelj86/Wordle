#pragma once

#include <string>
#include <vector>

#ifndef GAMESTATE_H
#define GAMESTATE_H
using namespace std;
class GameState {
 public:
  //=================== YOUR CODE HERE ===================
  // TODO: Add the constructor(s), accessors, mutators,
  // and any other member function you may need here.
  //======================================================
  GameState(const std::string& answer);
  
  
  std::string GetSecretTuffle();
  void SetSecretTuffle(const std::string &answer);
  std::string GetGameStatus();
  void SetGameStatus(const std::string &game_status);
  std::string GetMessage();
  void SetErrorMessage(const std::string &error_message);
  std::vector<string>& GetGuessedWords();
  void SetGuessedWords( std::vector<string> guesses_words);
  std::vector<string>& GetBoardColors();
  void SetBoardColors(const std::vector<string>  &board_colors);
   void ComparingTwoWords(bool is_there_no_error_);
std::string GetGuess();
  void SetGuess(const std::string &guess);
  std::string GetBoardColor();
  void SetBoardColor(const std::string &board_color);
  void Delete();
  void AddLetter(char c);

 private:
  //=================== YOUR CODE HERE ===================
  // TODO: Add any member variables you need here to store
  // the state of the game. What information needs to be
  // stored to fully represent the game state at a single
  // point in time?

   std::string guess_answer_ = "";
   std::string board_color_ = "";
    std::string secret_tuffle_ = "tuffy";
    std::vector<string> guessed_words_;
    std::string game_state_ = "active";
    std::vector<string> board_colors_;
    std::string error_message_ = "";
    int num_guesses_ = 0;  //======================================================
};

#endif  // GAMESTATE_H