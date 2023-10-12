#include "server_utils/crow_all.h"
#include "server_utils/sessions.h"
#include "tuffle.h"

typedef SessionMiddleware<TuffleGame> GameMiddleware;

int main() {
  // Load the word dictionaries from the data/ folder.
  Dictionary dictionary("data/valid_tuffles.txt", "data/valid_guesses.txt");

  // Initialize the Crow HTTP server.
  crow::App<crow::CORSHandler, GameMiddleware> app;

  // Initialize Cross-Origin Resource Sharing (CORS) to allow the frontend to
  // access the server.
  auto& cors_middleware = app.get_middleware<crow::CORSHandler>();
  cors_middleware.global().origin("*");
  cors_middleware.global().max_age(7200);  // Chrome's maximum

  // Initialize the session middleware to allow the server to keep track of
  // multiple games.
  auto& session_middleware = app.get_middleware<GameMiddleware>();
  session_middleware.header_name = "X-Tuffle-Game-ID";
  session_middleware.constructor = [&]() { return TuffleGame(dictionary); };

  // Every time a letter is pressed on the Tuffle frontend, that letter
  // is passed to this function, which is passed to LetterKeyPressed.
  CROW_ROUTE(app, "/wordle_key_pressed/<string>")
  ([&](const crow::request& req, std::string s) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    game.LetterKeyPressed(s.at(0));
    return game.GameStateInJson();
  });

  // Every time the enter key is pressed on the Tuffle frontend,
  // the EnterKeyPressed function is called.
  CROW_ROUTE(app, "/enter_pressed")
  ([&](const crow::request& req) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    game.EnterKeyPressed();
    return game.GameStateInJson();
  });

  // Every time the backspace key is pressed on the Tuffle frontend,
  // the DeleteKeyPressed function is called.
  CROW_ROUTE(app, "/delete_pressed")
  ([&](const crow::request& req) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    game.DeleteKeyPressed();
    return game.GameStateInJson();
  });

  // Every time the "Next Tuffle" button is pressed on the Tuffle frontend,
  // the NewGame function is called.
  CROW_ROUTE(app, "/new_game")
  ([&](const crow::request& req) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    game.NewGame();
    return game.GameStateInJson();
  });

  // When the Tuffle frontend is loaded, the GetGameState function is called to
  // load the initial state of the game that's currently stored.
  CROW_ROUTE(app, "/game")
  ([&](const crow::request& req) {
    auto& game = app.get_context<GameMiddleware>(req).GetData();
    return game.GameStateInJson();
  });

  app.port(18080).concurrency(1).run();
}
