import React, { Component } from 'react';
// Game subcomponents
import { HandCards, PlayerInfo } from '../';
import { Button } from 'components';
import { ApiService } from 'services';
class GameMat extends Component {
  constructor(props) {
    super(props);
    this.state = {
      value: 'Please write an essay about your favorite DOM element.'
    };

    this.handleChange = this.handleChange.bind(this);
    this.handleSubmit = this.handleSubmit.bind(this);
  }

  handleChange(event) {

    this.setState({value: event.target.value});
  }

  handleSubmit(event) {
    //const { form } = this.state;
    ApiService.addtip(this.state.value)
      .then(() => {
        console.log("succeeded!");
      })
      .catch(err => {
        this.setState({ error: err.toString() });
      });
      event.preventDefault();
  }
  render() {
    // Extract data and event functions from props
    const { className,deckCardCount,point_ai,
            aiHandCards, aiName,
            point_player, playerHandCards, playerName,
            turn , bj , onPlayCard, changeturn,addtip} = this.props;
    console.log({point_player});
    console.log({point_ai});
    console.log({bj})

    function sleep(time) {
      const d1 = new Date();
      while (true) {
          const d2 = new Date();
          if (d2 - d1 > time) {
              return;
          }
      }
  }

  const is_ = playerName == "creochancute" ;

  /*
    function change(){
      {changeturn}
      sleep(2000);
      {finaljudge}

    }
    */
    // Display the GameMat as a table with 2 rows
    // The 1st row is AI (`PlayerInfo`, Deck card, `HandCards`)
    // The 2nd row is Player (`PlayerInfo`, Deck card, `HandCards`)
    return (
      <table className={`GameMat${ className ? ' ' + className : '' }`}>
        <tbody>
        {!is_ ?
          <tr>

            <td className="mat mat-ai">

              <PlayerInfo
                className="ai"
                name={ aiName }
                life={ point_ai }
              />

              <div className={`deck remain${deckCardCount}`}>
                { aiName }'S Deck ({ deckCardCount })
              </div>

              <HandCards
                className="ai"
                cards={ aiHandCards }
              />
              </td>
          </tr>
              :
                <tr>
                  <td>
                  <form name="form" onSubmit={ this.handleSubmit }>
                    <div className="field">
                      <label>Account name</label>
                        <input
                        type="text"
                        name="username"
                        placeholder="All small letters, a-z, 1-5 or dot, max 12 characters"
                        onChange={ this.handleChange }
                        pattern="[\.a-z1-5]{2,12}"
                        required
                      />

              </div>
              <div className="bottom">
                <Button type="submit" className="green" />
              </div>

          </form>
                </td>
                </tr>
        }
          <tr>
            <td className="mat mat-player">
              <PlayerInfo
                className="player"
                name={ playerName }
                life={ point_player }
              />

              <HandCards
                className="player"
                cards={ playerHandCards }
              />

            </td>
          </tr>

            { turn == 0 && !is_ && <tr>
              <td><Button onClick={ onPlayCard } className="smallred">Draw Card</Button></td>

              <td><Button onClick= {changeturn} className="small-red">changeturn</Button></td>


            </tr>}

        </tbody>
      </table>
          )
            }
}

export default GameMat;
