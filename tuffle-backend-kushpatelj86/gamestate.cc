#include "gamestate.h"

// ========================= YOUR CODE HERE =========================
// This implementation file is where you should implement the member
// functions declared in the header, only if you didn't implement
// them inline in the header.
//
// Remember to specify the name of the class with :: in this format:
//     <return type> MyClassName::MyFunction() {
//        ...
//     }
// to tell the compiler that each function belongs to the GameState class.
// 


GameState::GameState(const std::string& answer)
{
  secret_tuffle_ = answer;
  game_state_ = "active";
  error_message_ = "";
  
  std::vector<string> guessed_words{};
  guessed_words_ = guessed_words;
  std::vector<string> board_colors{};
  board_colors_ = board_colors;
  

 



  
}

 string GameState::GetSecretTuffle()
{
  return secret_tuffle_;
}


void GameState::SetSecretTuffle(const std::string &answer)
{
  secret_tuffle_ = answer;
}

string GameState::GetGameStatus()
{
  return game_state_;
}


void GameState::SetGameStatus( const std::string &game_state)
{
   game_state_ = game_state;
}


std::string GameState::GetMessage()
{
  return error_message_;
}

void GameState::SetErrorMessage(const std::string &error_message)
{
  error_message_ = error_message;
}

std::vector<string>& GameState::GetGuessedWords()
{
  return guessed_words_;
}

std::vector<string>& GameState::GetBoardColors()
{
  return board_colors_;
}

void GameState::SetGuessedWords(std::vector<string> guesses_words)
{
  guessed_words_ = guesses_words;
}


void GameState::SetBoardColors(const std::vector<string> &board_colors)
{
  board_colors_ = board_colors;
}



void GameState::AddLetter(char c)
{
    if (guessed_words_.empty()) 
     {
      guessed_words_.push_back("");
    }


    if(guess_answer_.size() < 5)
    {
      guess_answer_ = guess_answer_ + c;
      guessed_words_[num_guesses_] = guess_answer_;
      error_message_ = "";
    }


  
}

void GameState::ComparingTwoWords(bool is_there_no_error_)
{
    error_message_ = "";
    if(guess_answer_.size() < 5)
    {
      error_message_ = "Not enough letters, try again.";

    }
    else if (!is_there_no_error_) 
    {
      error_message_ = "Invalid guess. Try again.";
    }
    else
    {
      error_message_ = "";
      std::string answer = secret_tuffle_;
      guessed_words_.push_back("");
      num_guesses_++;
      board_color_ = "BBBBB";


      for (int i = 0; i < answer.size(); i++) 
        {
          if (guess_answer_[i] == answer[i]) 
          {
            board_color_[i] = 'G';
          }
        }



      
       for (int i = 0; i < answer.size(); i++)
        {

          if (answer.find(guess_answer_[i]) != -1) 
          {
            if (board_color_[i] != 'G') 
            {
              board_color_[i] = 'Y';
            }
          }
        }


        


      
        
      
        board_colors_.push_back(board_color_);
        if (guess_answer_ == secret_tuffle_) 
        {
            game_state_ = "win";
        } 
        else if (guessed_words_.size() == 6 && guessed_words_[5] != secret_tuffle_) 
        {
            game_state_ = "lose";
        }
    guess_answer_.clear();

      
    }


  
}


std::string GameState::GetGuess()
{
  return guess_answer_;
}

void GameState::SetGuess(const std::string &guess)
{
  guess_answer_ = guess;
}


  std::string GameState::GetBoardColor()
{
  return board_color_;
}

  void GameState::SetBoardColor(const std::string &board_color)
{
  board_color_ = board_color;
}

void GameState::Delete()
{
  guess_answer_.pop_back();

  guessed_words_[num_guesses_] = guess_answer_;

  error_message_ = "";



  
}



