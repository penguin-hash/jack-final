import { ActionTypes } from 'const';

const initialState = {
  name: "",
  tip: 0,
  refund_tip: 0,
  refunding:0,
  game: null,
};

export default function (state = initialState, action) {
  switch (action.type) {
    case ActionTypes.SET_USER: {
      return Object.assign({}, state, {
        // If the name is not specified, do not change it
        // The places that will change the name is login
        // In that cases, the `win_count`, `lost_count`, `game` will be reset
        name: typeof action.name === "undefined" ? state.name : action.name,
        tip: action.tip || initialState.tip,
        refund_tip: action.refund_tip || initialState.refund_tip,
        refunding: action.refunding || initialState.refunding,
        game: action.game || initialState.game,
      });
    }
    default:
      return state;
  }
}
