#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Right, Left, Hold, Release };
enum class State { Playable, Wait, GameOver, Win };

struct GameData {
  State m_state { State::Playable };
  std::bitset<5> m_input;  // [Right, Left, Hold, Release]
};

#endif