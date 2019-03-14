import React, { Component } from 'react';
// Components
//import { HandCards, PlayerInfo } from '../';
import { Button } from 'components';
// Game subcomponents
import { RulesModal } from './components';

class Info extends Component {
  render() {
    // Extract data and event functions from props
    const { className, deckCardCount, handCardCount, onPlayCard,turn,changeturn,point_ai,
            aiHandCards, aiName,
            point_player, playerHandCards, playerName,
             } = this.props;


     // { turn == 0 && <Button onClick={ changeturn } className="small red" id="change">changeturn</Button>}
     // { turn == 0 && <Button onClick={ onPlayCard } className="small red" id="draw">Draw Card</Button>}
     // 
    // Display:
    // Round number: 18 <-- ((max deck = 17) + 1) - Deck Cards - Hand Cards
    // Rules button to trigger a modal
    // Button to end the current game
    return (
      <div className={`Info${ className ? ' ' + className : '' }`}>

        { turn == 0 && <p> <span className="round-number">YourTurn</span></p> }
        { turn == 1 && <p> <span className="round-number">DEALERTurn</span></p> }

      </div>

    )
  }
}

export default Info;
