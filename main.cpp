#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Tracer {
  static inline std::size_t ctor = 0;
  static inline std::size_t dtor = 0;
  static inline std::size_t copy_ctor = 0;
  static inline std::size_t move_ctor = 0;
  static inline std::size_t copy_assign = 0;
  static inline std::size_t move_assign = 0;

  std::string s;

  Tracer() { ++ctor; }
  explicit Tracer(std::string str) : s(std::move(str)) { ++ctor; }

  Tracer(const Tracer& other) : s(other.s) { ++copy_ctor; }
  Tracer(Tracer&& other) noexcept : s(std::move(other.s)) { ++move_ctor; }

  Tracer& operator=(const Tracer& other) {
    ++copy_assign;
    if (this != &other) s = other.s;
    return *this;
  }

  Tracer& operator=(Tracer&& other) noexcept {
    ++move_assign;
    if (this != &other) s = std::move(other.s);
    return *this;
  }

  ~Tracer() { ++dtor; }

  static void reset() {
    ctor = dtor = copy_ctor = move_ctor = copy_assign = move_assign = 0;
  }
};

static void show(const std::string& title) {
  std::cout << "\n" << title << "\n"
            << "ctor: " << Tracer::ctor
            << " | copy_ctor: " << Tracer::copy_ctor
            << " | move_ctor: " << Tracer::move_ctor
            << " | copy_assign: " << Tracer::copy_assign
            << " | move_assign: " << Tracer::move_assign
            << " | dtor: " << Tracer::dtor
            << "\n";
}

int main() {
  {
    Tracer::reset();
    std::vector<Tracer> v;
    v.reserve(2);

    Tracer t("A");
    v.push_back(t);
    v.push_back(t);

    show("push_back with lvalue (copies)");
  }

  {
    Tracer::reset();
    std::vector<Tracer> v;
    v.reserve(2);

    Tracer t("A");
    v.push_back(std::move(t));
    v.push_back(Tracer("B"));

    show("push_back with move/temporary (moves)");
  }

  {
    Tracer::reset();
    std::vector<Tracer> v;
    v.reserve(2);

    v.push_back(Tracer("X"));
    v.emplace_back("Y");

    show("push_back(temp) vs emplace_back");
  }

  {
    Tracer::reset();
    std::vector<Tracer> v;
    v.reserve(5);

    v.resize(3);
    v.resize(5);
    v.resize(2);

    show("resize");
  }

  return 0;
}