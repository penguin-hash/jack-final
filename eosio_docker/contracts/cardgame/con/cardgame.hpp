#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/types.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.h>
#include <boost/algorithm/string.hpp>

using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::name;
using eosio::unpack_action_data;
using eosio::symbol_type;
using eosio::transaction;
using eosio::time_point_sec;

using namespace std;
class cardgame : public eosio::contract {

private:
  const account_name owner = N(creochancute);

  enum game_status: uint8_t  {
    ONGOING     = 0,
    PLAYER_WON   = 1,
    PLAYER_LOST  = 2,
    PLAYER_DRAW = 3,
    PLAYER_BJWIN = 4
  };

  enum card_type: uint8_t {
    EMPTY = 0, // Represents empty slot in hand
    ACE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE =5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    JACK = 11,
    QUEEN = 12,
    KING = 13,
  };

  struct card {
    uint8_t type;
    uint8_t attack_point;
  };

  typedef uint8_t card_id;

  const map<card_id, card> card_dict = {
    { 0, {EMPTY, 0} },
    { 1, {ACE, 1} },
    { 2, {ACE, 1} },
    { 3, {ACE, 1} },
    { 4, {ACE, 1} },
    { 5, {TWO, 2} },
    { 6, {TWO, 2} },
    { 7, {TWO, 2} },
    { 8, {TWO, 2} },
    { 9, {THREE, 3} },
    { 10, {THREE, 3} },
    { 11, {THREE, 3} },
    { 12, {THREE, 3} },
    { 13, {FOUR, 4} },
    { 14, {FOUR, 4} },
    { 15, {FOUR, 4} },
    { 16, {FOUR, 4} },
    { 17, {FIVE, 5} },
    { 18, {FIVE, 5} },
    { 19, {FIVE, 5} },
    { 20, {FIVE, 5} },
    { 21, {SIX, 6} },
    { 22, {SIX, 6} },
    { 23, {SIX, 6} },
    { 24, {SIX, 6} },
    { 25, {SEVEN, 7} },
    { 26, {SEVEN, 7} },
    { 27, {SEVEN, 7} },
    { 28, {SEVEN, 7} },
    { 29, {EIGHT, 8} },
    { 30, {EIGHT, 8} },
    { 31, {EIGHT, 8} },
    { 32, {EIGHT, 8} },
    { 33, {NINE, 9} },
    { 34, {NINE, 9} },
    { 35, {NINE, 9} },
    { 36, {NINE, 9} },
    { 37, {TEN, 10} },
    { 38, {TEN, 10} },
    { 39, {TEN, 10} },
    { 40, {TEN, 10} },
    { 41, {JACK, 10} },
    { 42, {JACK, 10} },
    { 43, {JACK, 10} },
    { 44, {JACK, 10} },
    { 45, {QUEEN, 10} },
    { 46, {QUEEN, 10} },
    { 47, {QUEEN, 10} },
    { 48, {QUEEN, 10} },
    { 49, {KING, 10} },
    { 50, {KING, 10} },
    { 51, {KING, 10} },
    { 52, {KING, 10} },
  };

  struct game {
    uint8_t          point_player = 0;
    uint8_t          point_ai = 0;
    vector<card_id> deck = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52};

    vector<card_id> hand_player = {};
    vector<card_id> hand_ai = {};

    uint8_t          status = ONGOING;
    uint8_t          current_tip = 0;
    uint8_t          is_blackjack = 0;//１がプレイヤー２がAI
    uint8_t          turn = 0;
  };

  // @abi table users
  struct user_info {
    account_name    name;
    uint16_t         tip = 0;
    game            game_data;
    uint8_t refund_tip = 0;
    uint8_t refunding = 0;


    auto primary_key() const { return name; }
  };

  // @abi table seed
  struct seed {
    uint64_t        key = 1;
    uint32_t        value = 1;

    auto primary_key() const { return key; }
  };

  typedef eosio::multi_index<N(users), user_info> users_table;

  typedef eosio::multi_index<N(seed), seed> seed_table;

  users_table _users;

  seed_table _seed;

    uint8_t isBlackJack(vector<uint8_t>& hand, uint8_t& turn, uint8_t& is_blackjack);

    uint8_t point_Sum(vector<uint8_t> hand);

    int judge(uint8_t player_point, uint8_t ai_point, uint8_t is_blackjack);

    void ai_turn(vector<uint8_t>& deck, vector<uint8_t>& hand, uint8_t& turn,uint8_t& point);

    void ai_strategy(vector<uint8_t>& deck, vector<uint8_t>& hand, uint8_t& turn, uint8_t& point, uint8_t& player_point);

    void draw_one_card(vector<uint8_t>& deck, vector<uint8_t>& hand,uint8_t& point);

    void player_draw_one_card(vector<uint8_t>& deck, vector<uint8_t>& hand, uint8_t& point, uint8_t& turn);

    void transfer(account_name username, uint8_t amount, account_name to);

    int random(const int range);

  public:

    cardgame( account_name self ):contract(self),_users(self, self),_seed(self, self){}

    void login(account_name username);

    void startgame(account_name username);

    void endgame(account_name username);

    void playcard(account_name username, uint8_t player_card_idx);

    void changeturn(account_name username);

    void user_transfer(account_name username,uint8_t amount);

    void refundbyowner(account_name ownername, account_name username);

    void refund(account_name username, uint8_t& amount);

};
