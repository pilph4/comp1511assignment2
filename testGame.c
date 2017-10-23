//this is probably the most important part of the assignment - once this is
//working, we should understand the other things well enough to implement them
//fairly easily.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Game.h"

void testTheGame (void);

int main(int argc, char *argv[]){
    printf("Today, we're going to be running some tests.\nHopefully they will work.\n");
    testTheGame();

    printf("If you've reached this part, then the tests were succesful. Hurrah!\n");
}

void testTheGame (void) {
    value deckValues[28];
    color deckColors[28];
    suit deckSuits[28];

    int i = 0;
        while (i < 28) {
            deckValues[i] = 1;
            deckSuits[i] = HEARTS;
            deckColors[i] = RED;
            i++;
        }
    Game game = newGame(28, deckValues, deckColors, deckSuits);
    assert(game != NULL);
    assert(currentPlayer(game) == 0);
    assert(currentTurn(game) == 0);
    // sure, these are just the same as Andrew's test ones, which he said not to
    // copy, but these are such simple and important tests that it would be silly
    // not to put them in here.

    // select a card from the first player's hand to allocate as the card
    // we will play

    Card firstCard = handCard(game,0);

    playerMove move;
    move.action = PLAY_CARD;
    move.card = firstCard;
    assert(isValidMove(game, move) == TRUE);
    playMove (game, move);

    assert(currentPlayer(game) == 1);
    assert(currentTurn(game) == 1);
    assert(playerCardCount(game, 0) == 6);

}
