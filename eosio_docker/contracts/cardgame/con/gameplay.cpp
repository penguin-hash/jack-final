#include "cardgame.hpp"

// Simple Pseudo Random Number Algorithm, randomly pick a number within 0 to n-1
int random(const int range) {
  // Find the existing seed
  auto seed_iterator = _seed.begin();

  // Initialize the seed with default value if it is not found
  if (seed_iterator == _seed.end()) {
    seed_iterator = _seed.emplace( _self, [&]( auto& seed ) { });
  }

  // Generate new seed value using the existing seed value
  int prime = 65537;
  auto new_seed_value = (seed_iterator->value + now()) % prime;

  // Store the updated seed value in the table
  _seed.modify( seed_iterator, _self, [&]( auto& s ) {
    s.value = new_seed_value;
  });

  // Get the random result in desired range
  int random_result = new_seed_value % range;
  return random_result;
}
uint8_t isBlackJack(vector<uint8_t>& hand, uint8_t& turn, uint8_t& is_blackjack){
  eosio_assert(hand.size() == 2, "It's not BlackJack");
  uint8_t hand_sum = point_Sum(hand);
  if(hand_sum == 21 && turn == 0){
      is_blackjack = 1;
  }else if(hand_sum == 21 && turn == 1 && is_blackjack == 1){
      is_blackjack = 3;
  }else if(hand_sum == 21 && turn == 0){
      is_blackjack = 2;
  }
  return is_blackjack;
}

uint8_t point_Sum(vector<uint8_t> hand){
  uint8_t point = 0;
  for(int i; i < hand.size(); i++){
    const auto card_id = hand[i];
    const auto card = card_dict.at(card_id);

      if(card.type == ACE && point + 11 < 21){
          point = point + 11;
      }else{
      point = point + card.attack_point;
      }
  }
  return point;
}

int judge(uint8_t player_point, uint8_t ai_point, uint8_t is_blackjack){
  if(is_blackjack == 1){//BJかち
    return 4;
  }else if(is_blackjack == 2){//負け
    return 2;
  } else if(is_blackjack == 3){//引き分け
    return 1;
  } else if (player_point < 22 && player_point > ai_point){//かち
    return 3;
  } else if(player_point < 22 && player_point == ai_point){//引き分け
    return 1;
  }else if(player_point < 22 && player_point < ai_point){//負け
    return 2;
  }else if(player_point > 21){//負け
    return 2;
  }
}

void ai_turn(vector<uint8_t>& deck, vector<uint8_t>& hand, uint8_t& turn,uint8_t& point){
  eosio_assert(turn == 1, "It's not AI turn");
  eosio_assert(hand.size() == 1, "It's not Size");
  draw_one_card(deck, hand,point);
}

void ai_strategy(vector<uint8_t>& deck, vector<uint8_t>& hand, uint8_t& turn, uint8_t& point, uint8_t& player_point){
    eosio_assert(turn == 1, "It's not AI turn");
    eosio_assert(hand.size() == 2, "It's not Size");
    while (point > 17){
      draw_one_card(deck,hand,point);
    }
    if(player_point > point){
      draw_one_card(deck,hand,point);
    }
}
// Draw one card from the deck and assign it to the hand
void draw_one_card(vector<uint8_t>& deck, vector<uint8_t>& hand,uint8_t& point) {
  // Pick a random card from the deck
  int deck_card_idx = random(deck.size());
  //eosio_assert(first_empty_slot != -1, "No empty slot in the player's hand");

  // Assign the card to the first empty slot in the hand
  hand.push_back(deck_card_idx);
  point = point_Sum(hand);

  // Remove the card from the deck
  deck.erase(deck.begin() + deck_card_idx);
}

void player_draw_one_card(vector<uint8_t>& deck, vector<uint8_t>& hand, uint8_t& point, uint8_t& turn) {
  // Pick a random card from the deck for player.
  eosio_assert(turn == 0, "It's not your turn");
  eosio_assert(turn == 1, "It's not your turn");
  eosio_assert(point <  21, "Already Bust!!");

  int deck_card_idx = random(deck.size());
  //eosio_assert(first_empty_slot != -1, "No empty slot in the player's hand");

  // Assign the card to the first empty slot in the hand
  hand.push_back(deck[deck_card_idx]);
  point = point_Sum(hand);
  // Remove the card from the deck
  deck.erase(deck.begin() + deck_card_idx);
}

// Calculate the final attack point of a card after taking the elemental bonus into account
/*
int cardgame::calculate_attack_point(const card& card1, const card& card2) {
  int result = card1.attack_point;

  //Add elemental compatibility bonus of 1
  if ((card1.type == FIRE && card2.type == WOOD) ||
      (card1.type == WOOD && card2.type == WATER) ||
      (card1.type == WATER && card2.type == FIRE)) {
    result++;
  }

  return result;
}
*/
void transfer(account_name username, uint8_t amount, account_name to){
        require_auth(username);
        eosio_assert(amount < 100, "too much EOS");
        action(
				permission_level{_self, N(active)},
				N(eosio.token),
				N(transfer),
				std::make_tuple(
                    permission_level{_self, N(active)},
					_self,
					to,
					asset(amount, symbol_type(S(4, EOS))),
					std::string(" -- Transfered.")
				)
			).send();
    }
