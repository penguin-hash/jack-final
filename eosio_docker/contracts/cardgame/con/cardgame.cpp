#include "gameplay.cpp"

cardgame( account_name self ):contract(self),_users(self, self),_seed(self, self){}

void login(account_name username) {
// Ensure this action is authorized by the player
    require_auth(username);

// Create a record in the table if the player doesn't exist in our app yet
    auto user_iterator = _users.find(username);
    if (user_iterator == _users.end()) {
    user_iterator = _users.emplace(username,  [&](auto& new_user) {
    new_user.name = username;
    //new_user.ui_name = N(username);
    });
}
}

void startgame(account_name username, uint8_t _current_tip) {
// Ensure this action is authorized by the player
    require_auth(username);

    auto& user = _users.get(username, "User doesn't exist");

    _users.modify(user, username, [&](auto& modified_user) {
// Create a new game
    game game_data;
    eosio_assert( user.tip < _current_tip , "Current tip is larger than your tip.");
    modified_user.tip = modified_user.tip - _current_tip;
    game_data.current_tip = _current_tip;

// Draw 2 cards each for the player
    for (uint8_t i = 0; i < 2; i++) {
        draw_one_card(game_data.deck, game_data.hand_player,game_data.point_player);
    }
    // Draw 1 cards(open) each for AI
    draw_one_card(game_data.deck, game_data.hand_ai,game_data.point_ai);
    game_data.is_blackjack = isBlackJack(game_data.hand_player,game_data.turn, modified_user.game_data.is_blackjack);

// Assign the newly created game to the player
    modified_user.game_data = game_data;
});
}


void endgame(account_name username) {
    // Ensure this action is authorized by the player
    require_auth(username);

    // Get the user and reset the game
    auto& user = _users.get(username, "User doesn't exist");
    _users.modify(user, username, [&](auto& modified_user) {
    //eosio_assert(modified_user.game_data.turn == 1,'your turn');

    if(modified_user.game_data.status == PLAYER_DRAW){//引き分け
        //statusを入れる
        modified_user.tip = modified_user.tip + modified_user.game_data.current_tip;
        modified_user.game_data.current_tip = 0;
        modified_user.game_data = game();

    }else if(modified_user.game_data.status == PLAYER_LOST){//負け
        modified_user.game_data.current_tip = 0;
        modified_user.game_data = game();

    }else if(modified_user.game_data.status == PLAYER_WON){//かち
        modified_user.tip = modified_user.tip + modified_user.game_data.current_tip * 2;
        modified_user.game_data.current_tip = 0;
        modified_user.game_data = game();

    }else if(modified_user.game_data.status == PLAYER_BJWIN){//かち
        modified_user.tip = modified_user.tip + modified_user.game_data.current_tip * 2.5;
        modified_user.game_data.current_tip = 0;
        modified_user.game_data = game();

    }
    });
}

void playcard(account_name username) {
    // Ensure this action is authorized by the player
    require_auth(username);

    auto& user = _users.get(username, "User doesn't exist");

    // Verify game status is suitable for the player to play a card
    eosio_assert(user.game_data.status == ONGOING,
           "playcard: This game has ended. Please start a new one");
    eosio_assert(user.game_data.turn == 0, "It's not your turn!");
    _users.modify(user, username, [&](auto& modified_user) {
        game& game_data = modified_user.game_data;

        // Assign the selected card from the player's hand

        // AI picks a card

        draw_one_card(game_data.deck, game_data.hand_player,game_data.point_player);
    });
    }

void changeturn(account_name username){
    require_auth(username);

// Get the user and reset the game
    auto& user = _users.get(username, "User doesn't exist");
    _users.modify(user, username, [&](auto& modified_user) {
    eosio_assert(modified_user.game_data.turn == 0, "It's Already AI turn");
    modified_user.game_data.turn = 1;
    ai_turn(modified_user.game_data.deck,modified_user.game_data.hand_ai, modified_user.game_data.turn,modified_user.game_data.point_ai);
    modified_user.game_data.is_blackjack = isBlackJack(modified_user.game_data.hand_player,modified_user.game_data.turn, modified_user.game_data.is_blackjack);
    ai_strategy(modified_user.game_data.deck, modified_user.game_data.hand_ai, modified_user.game_data.turn,
    modified_user.game_data.point_ai, modified_user.game_data.point_player);
    int win = judge(modified_user.game_data.point_ai, modified_user.game_data.point_player,modified_user.game_data.is_blackjack);
    if(win ==  1){//引き分け
        //statusを入れる

        modified_user.game_data.status = PLAYER_DRAW;
    }else if(win == 2){//負け

        modified_user.game_data.status = PLAYER_LOST;
    }else if(win == 3){//かち

        modified_user.game_data.status = PLAYER_WON;
    }else if(win == 4){//かち

        modified_user.game_data.status = PLAYER_BJWIN;
    }
});
}

void user_transfer(account_name username,uint8_t amount){
    require_auth(username);
    auto& user = _users.get(username, "User doesn't exist");
    _users.modify(user, username, [&](auto& modified_user) {
    transfer(username, amount, owner);
    modified_user.tip = modified_user.tip + amount;
    });
}

void refundbyowner(account_name ownername, account_name username){
    require_auth(ownername);
    eosio_assert(ownername == owner,"it's only by owner");
    auto& user = _users.get(username, "User doesn't exist");
    _users.modify(user, username, [&](auto& modified_user) {
    eosio_assert(modified_user.refunding == 1,"should not refund");
    transfer(ownername, modified_user.refund_tip, username);

    modified_user.refund_tip = modified_user.refund_tip - modified_user.refund_tip;
    });
}

void refund(account_name username, uint8_t& amount){
    require_auth(username);
    auto& user = _users.get(username, "User doesn't exist");
    _users.modify(user, username, [&](auto& modified_user) {
    eosio_assert(modified_user.refunding == 0,"can not refund");
    eosio_assert(modified_user.refund_tip == 0,"Can not refund");
    eosio_assert(modified_user.tip > amount,"Can not refund");

    modified_user.tip = modified_user.tip - amount;
    modified_user.refund_tip = amount;
    modified_user.refunding = 1;

    });
}


EOSIO_ABI(cardgame, (login)(startgame)(playcard)(changeturn)(endgame)(refundbyowner)(user_transfer)(refund))
