#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "crow_all.h"

template <class T>
struct SessionMiddleware {
  struct Session {
    Session(T new_data)
        : token(GenerateToken()),
          time(std::chrono::system_clock::now()),
          data(std::move(new_data)) {}

    void renew() { time = std::chrono::system_clock::now(); }

    const std::string token;
    std::chrono::time_point<std::chrono::system_clock> time;
    T data;
  };

  // context is a per-request object that's used by Crow.
  struct context {
    T& GetData() { return s_->data; }
    std::shared_ptr<Session> GetSession() { return s_; }

   private:
    // s_ is a pointer because Crow needs context to be zero-initializable.
    // We'll just have to juggle pointers.
    std::shared_ptr<Session> s_;
    friend struct SessionMiddleware;
  };

  void before_handle(crow::request& req, crow::response&, context& ctx) {
    if (req.method == crow::HTTPMethod::OPTIONS) {
      // Ignore CORS preflight here.
      return;
    }

    const auto session_id = req.get_header_value(header_name);
    if (auto it = sessions_.find(session_id); it != sessions_.end()) {
      ctx.s_ = it->second;
      ctx.s_->renew();
      return;
    }

    auto session = std::make_shared<Session>(constructor());
    sessions_[session->token] = session;
    ctx.s_ = session;
  }

  void after_handle(crow::request&, crow::response& resp, context& ctx) {
    if (ctx.s_ != nullptr) {
      // TODO: it's better to make a middleware for this, since we should only
      // have one of this header.
      resp.add_header("Access-Control-Expose-Headers", header_name);
      resp.set_header(header_name, ctx.s_->token);
    }

    // Remove other expired sessions.
    CleanupSessions();
  }

  std::string header_name = "X-Session-ID";
  std::chrono::seconds max_age = std::chrono::hours(96);
  std::function<T()> constructor;

 private:
  std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;

  void CleanupSessions() {
    const auto now = std::chrono::system_clock::now();
    auto it = sessions_.begin();
    while (it != sessions_.end()) {
      // Check if the session's age is greater than the max age.
      if ((now - it->second->time) > max_age) {
        // erase will automatically grab the next valid iterator.
        it = sessions_.erase(it);
      } else {
        ++it;
      }
    }
  }

  static std::string GenerateToken() {
    static std::atomic_int x(0);
    const auto t = std::chrono::high_resolution_clock::now();
    const auto ms = t.time_since_epoch() / std::chrono::milliseconds(1);
    return std::to_string(x++) + ":" + std::to_string(ms);
  }
};
