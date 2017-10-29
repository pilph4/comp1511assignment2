// game.c
// this is going to be interesting....
// let's see how this goes.

/*
so we have to make a struct which contains all the information needed for the game.
what'll that be?

we need to determine what cards are being used and what aren't, so that we can
only deal out cards that haven't been already dealt out (that's going to be fun.)

also need to assign which cards are in who's hands. that'll also not be very fun
there are 800 cards.

So a card can be in one of three states:
- in the deck
- in a player's hand (four options)
- or in the discard pile

These values are important to implement

It would seem that we make the deck at the beginning of the game; it is then
recorded in terms of three arrays - one of all the values, in order, one of the
suits, and one of the colors. This is a nice way of doing it. Phew.

a good thing to try and plan out would be a way of mapping a number between 0
and 799 to a card, so that I have my own way of making a deck.

*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Card.h"
#include "Game.h"

#define FIRST_DISCARD 28


typedef struct _cardlist *cardList;
typedef struct _cardnode *cardNode;


typedef struct _historylist *historyList;
typedef struct _historynode *historyNode;


typedef struct _movelist *moveList;
typedef struct _movenode *moveNode;


typedef struct _cardlist {
    cardNode head;
} cardlist;


typedef struct _cardnode {
    Card card;
    cardNode next;
} cardnode;

typedef struct _historylist {
    historyNode head;
} historylist;


typedef struct _historynode {
    int turnNumber;
    int playerNumber;
    moveList moves;
    historyNode next;
} historynode;


typedef struct _movelist {
    moveNode head;
} movelist;


typedef struct _movenode {
    playerMove move;
    moveNode next;
} movenode;


typedef struct _player {
    cardList hand;
    int points;
} player;


typedef struct _game {
    // info on the original deck
    int totalDeckSize;
    cardList originalDeck;


    // the games active components
    cardList draw;
    cardList discard;
    int turnNumber;
    int currentPlayer;
    int previousPlayer;
    direction currentDirection;
    int winStatus;


    // info relevant to each individual player
    player playerInfo[NUM_PLAYERS];


    // the move history
    historyList history;


} game;


// actions on card lists

static Card getNthCard(cardList list, int n);
static void putCardTopOfList(cardList list, Card card);
static void putCardEndOfList(cardList list, Card card);
static void removeCardFromHand(cardList list, Card card);
static void removeTopCard(cardList list);

static void freeEntireList(cardList list);
static void freeHistoryList(historyList list);
static void recordInHistory(Game game, playerMove move);
static void freeMoveList(moveList list);

static void drawFourCards(Game game);
static void drawTwoCards(Game game);

static void flipDiscardIntoDraw(Game game);

static int checkDrawTwoNonsense(Game game, playerMove move);

static int cardsMatch(Card first, Card second);

static int checkPreviousMoves(Game game, playerMove move);



Game newGame(int deckSize, value values[], color colors[], suit suits[]){
// deep breaths. let's go.
    Game new = calloc(1, sizeof(game));
    new->totalDeckSize = deckSize;
    new->turnNumber = 0;
    new->currentPlayer = 0;
    new->previousPlayer = 0;
    new->winStatus = NOT_FINISHED;
    new->currentDirection = CLOCKWISE;
// so far, so good.
    new->originalDeck = calloc(1, sizeof(cardList));
    new->originalDeck->head = NULL;

    new->draw = calloc(1, sizeof(cardList));
    new->draw->head = NULL;

    new->discard = calloc(1, sizeof(cardList));
    new->discard->head = NULL;

// but those are only the heads of the lists
// now we need to set up all the following items
// so first, the copy of the original deckSize

    int originalDeckCounter = 0;
    int counter = 0;
    while(counter < deckSize) {
        Card toAssign = newCard(values[originalDeckCounter],
            colors[originalDeckCounter], suits[originalDeckCounter]);
        putCardEndOfList(new->originalDeck, toAssign);
        originalDeckCounter++;
    }

// then the draw pile (original deck, minus 28 cards for the players hands,
// and another card for the discard pile)

    int drawCounter = 0;
    while(drawCounter < FIRST_DISCARD) {
        Card toAssign = newCard(values[drawCounter],
            colors[drawCounter], suits[drawCounter]);
        putCardEndOfList(new->draw, toAssign);
        drawCounter++;
    }

// for the discard pile, we just take the 28th (29th, actually) card
    Card toAssign = newCard(values[FIRST_DISCARD], colors[FIRST_DISCARD],
        suits[FIRST_DISCARD]);
    putCardTopOfList(new->discard, toAssign);



    int playerCounter = 0;
    while(playerCounter < NUM_PLAYERS) {
        new->playerInfo[playerCounter].points = 0;
        new->playerInfo[playerCounter].hand->head = NULL;
        playerCounter++;
    }

    int dealingCounter = 0;
    while(dealingCounter < FIRST_DISCARD) {
        Card toAssign = newCard(values[dealingCounter],
            colors[dealingCounter], suits[dealingCounter]);
        putCardTopOfList(new->playerInfo[dealingCounter%NUM_PLAYERS].hand, toAssign);
        dealingCounter++;
    }

    return new;



}

void destroyGame(Game game){ // i like how we get penalised for
                            // one-letter variables, but the prototype
                            // you give us has a one-letter variable.
                            // unless this is some sort of test....
                            // sneaky.
    freeEntireList(game->originalDeck);
    freeEntireList(game->draw);
    freeEntireList(game->discard);
    freeHistoryList(game->history);
    int playerCounter = 0;
    while(playerCounter < NUM_PLAYERS) {
        freeEntireList(game->playerInfo[playerCounter].hand);
        playerCounter++;
    }

    free(game);


}

// functions that have been implemented

int numCards(Game game){
    return game->totalDeckSize;
}

int numOfSuit(Game game, suit suit){
    cardNode curr = game->originalDeck->head;
    int suitCounter = 0;
    while(curr->next != NULL) {
        if(cardSuit(curr->card) == suit) {
            suitCounter++;
            curr = curr->next;
        } else {
            curr = curr->next;
        }
    }

    return suitCounter;
}

int numOfColor(Game game, color color){
    cardNode curr = game->originalDeck->head;
    int colorCounter = 0;
    while(curr->next != NULL) {
        if(cardColor(curr->card) == color) {
            colorCounter++;
            curr = curr->next;
        } else {
            curr = curr->next;
        }
    }
    return colorCounter;
}

int numOfValue(Game game, value value){
    cardNode curr = game->originalDeck->head;
    int valueCounter = 0;
    while(curr->next != NULL) {
        if(cardValue(curr->card) == value) {
            valueCounter++;
            curr = curr->next;
        } else {
            curr = curr->next;
        }
    }
    return valueCounter;
}

int currentPlayer(Game game){
    return game->currentPlayer;
}

int currentTurn(Game game){
    return game->turnNumber;
}

// NOT to be called by players
int playerPoints(Game game, int player){
    return game->playerInfo[player].points;
}

direction playDirection(Game game){
    return game->currentDirection;
}

Card topDiscard(Game game){
    assert(game->discard->head->next != NULL);
    return(game->discard->head->next->card);
}

Card handCard(Game game, int n) {
    int counter = 0;
    cardNode curr = game->playerInfo[currentPlayer(game)].hand->head->next;
    while(counter < n) {
        curr = curr->next;
    }
    Card card = curr->card;

    return card;
}

int playerCardCount(Game game, int player){
    int counter = 0;
    if(game->playerInfo[player].hand->head == NULL) {
        counter = 0;
    } else {
        counter++; // to account for the offset
        cardNode curr = game->playerInfo[player].hand->head;
        while(curr->next != NULL) {
            curr = curr->next;
            counter++;

        }
    }
    return counter;

}

int handCardCount(Game game){
    int player = game->currentPlayer;
    return playerCardCount(game->playerInfo[player].hand);

}




int numTurns(Game game) {
    return currentTurn(game)-1;
}



playerMove pastMove(Game game, int turn, int move) {
    assert(turn <= numTurns(game));
    int moveCounter = 0;
    playerMove moveMade;
    if(game->history->head == NULL) {
        // do nothing
    } else {
        historyNode curr = game->history->head;
        while(curr->turnNumber != turn) {
            curr = curr->next;
        }
        moveNode mCurr = curr->moves->head;
        while(moveCounter != move) {
            mCurr = mCurr->next;
        }
        moveMade = mCurr->move;
    }
    return moveMade;
}

int turnMoves(Game game, int turn) {
    assert(turn <= numTurns(game) && turn >= 0);
    int moveListSize = 0;
    historyNode curr = game->history->head;
    while(curr->turnNumber != turn) {
        curr = curr->next;
    }
    moveNode moveCurr = curr->moves->head;
    while(moveCurr->next != NULL) {
        moveListSize++;
        moveCurr = moveCurr->next;
    }
    return moveListSize;
}


// functions yet to be implemented fully


int gameWinner(Game game) {
    int playerCounter = 0;
    int winner = NOT_WINNER;
    while(playerCounter < 4) {
        if
    }
}





int isValidMove(Game game, playerMove move){
    // aaaaaaaaaaaaaaaaaaaaaaa.
    // deep breaths.
    // let's go.
    int valid = TRUE;
    Card topOfDiscard = topDiscard(game);



    valid = checkPreviousMoves(game, move);

    if(cardValue(topOfDiscard) == DRAW_TWO) {
        valid = checkDrawTwoNonsense(game, move);
        // to deal with all the 'draw two'
        // shenanigans and history diving
    }


    if(move.action == PLAY_CARD && valid == TRUE) {
        if(move.card == NULL) {
            valid = FALSE;
        } else {
            if(cardValue(move.card) == ZERO) {
                // all is well - unless it's a draw two?
                if(cardValue(topOfDiscard) ==  DRAW_TWO) {
                    valid = FALSE;
                }
            } else if(cardsMatch(move.card, topOfDiscard) != TRUE) {
                valid = FALSE;
                if(cardValue(topOfDiscard) == DECLARE) {
                    if(cardColor(topOfDiscard) == game->history->head->moves->head->color) {
                        valid = TRUE;
                    }
            } else if() {

            } else if() {

            }
        }
    }


    if(move.action == DRAW_CARD && valid == TRUE) {
        if(topOfDiscard == DRAW_TWO) {
            valid = checkDrawTwoNonsense(game, move);

        }
    }

    if(move.action == END_TURN && valid == TRUE) {
        if(game->history->head->moves->head->move == END_TURN) {
            // is this convoluted enough yet?
            valid = FALSE;
        }
    }

    if(move.action [][][])









        // aaaaaaaaaaaaaaaaaaaaaaa how do we deal with drawing?
        // because i don't know if they've drawn both of their cards yet
        // so we delve into the move history!!
        // aha!





    if()








    return valid;
}







void playMove(Game game, playerMove move){

//    assert(isValidMove(game, move));
    cardList playersHand = game->playerInfo[currentPlayer(game)].hand;
    int skipped = FALSE;
    int current = game->currentPlayer;
    int previous = game->previousPlayer;
    if(move.action == DRAW_CARD) {
        Card topOfDraw = getNthCard(game->draw, 1);
        putCardTopOfList(playersHand, topOfDraw);
        removeTopCard(game->draw);
    }
    if(move.action == PLAY_CARD) {
        putCardTopOfList(game->discard, move.card);
        removeCardFromHand(playersHand, move.card);
        if(cardValue(move.card) == DECLARE) {
            // [][][] does anything need to be put here, or is
            // this only relevant for the person whose turn is next?
        } else if(cardValue(move.card) == ADVANCE) {
            skipped = TRUE;
        } else if(cardValue(move.card) == BACKWARDS) {
            if(game->currentDirection == CLOCKWISE) {
                game->currentDirection = ANTICLOCKWISE;
            } else {
                game->currentDirection = CLOCKWISE;
            }



    }
    if(move.action == SAY_UNO) {
        printf("UNO!\n");
        if(playerCardCount(game->playerInfo[current].hand) == 1) {
            // well done! you've almost won!
        } else if (playerCardCount(game->playerInfo[previous].hand) == 1) {
            // silly billy!
            drawFourCards(game);
        }
        else {
            // oops
            drawTwoCards(game);
        }
    }


    if(move.action == SAY_DUO) {
        printf("DUO!\n");
        if(playerCardCount(game->playerInfo[current].hand) == 2) {
            // well done! you've almost won!
        } else if (playerCardCount(game->playerInfo[previous].hand) == 2) {
            // silly billy!
            drawFourCards(game);
        }
        else {
            // oops
            drawTwoCards(game);
        }
    }


    if(move.action == SAY_TRIO) {
        printf("TRIO!\n");
        if(playerCardCount(game->playerInfo[current].hand) == 3) {
            // well done! you've almost won!
        } else if (playerCardCount(game->playerInfo[previous].hand) == 3) {
            // silly billy!
            drawFourCards(game);
        }
        else {
            // oops
            drawTwoCards(game);
        }
    }


    if(move.action == END_TURN) {
        game->turnNumber++;
        game->previousPlayer = game->currentPlayer;
        if(game->currentDirection == CLOCKWISE) {
            game->currentPlayer = (game->currentPlayer+1)%4;
        } else {
            game->currentPlayer = (game->currentPlayer-1)%4;
        }
        if(skipped == TRUE) {
            if(game->currentDirection == CLOCKWISE) {
                game->currentPlayer = (game->currentPlayer+1)%4;
            } else {
                game->currentPlayer = (game->currentPlayer-1)%4;
            }
        }
    }




    recordInHistory(game, move);


// i'm not sure if this is totally finished yet, but it's better than before
// [][][]

    }


}

// my own functions, to do with card and history lists








static void removeCardFromHand(cardList list, Card card) {
    assert(list->head != NULL);
    cardNode prev = list->head;
    cardNode curr = list->head;
    while(curr->card != card && curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }
    prev->next = curr->next;
    destroyCard(curr->card);
    free(curr);
}







static void removeTopCard(cardList list){
    assert(list->head != NULL);
    cardNode topCard = list->head->next;
    cardNode nextCard = topCard->next;
    list->head = nextCard;
    destroyCard(topCard->card);
    free(topCard);
}



static void reviewPoints(Game game) {
    int playerCounter = 0;
    while(playerCounter < 4) {
        int pointCounter = 0;
        cardNode playerHand = game->playerInfo[playerCounter].hand->head;
        while(playerHand->next != NULL) {
            int tempCardPoint = 0;
            if(cardValue(playerHand->card) == ZERO) {
                tempCardPoint = 0;
            }
            if(cardValue(playerHand->card) == ONE) {
                tempCardPoint = 1;
            }
            if(cardValue(playerHand->card) == DRAW_TWO) {
                tempCardPoint = 2;
            }
            if(cardValue(playerHand->card) == THREE) {
                tempCardPoint = 3;
            }
            if(cardValue(playerHand->card) == FOUR) {
                tempCardPoint = 4;
            }
            if(cardValue(playerHand->card) == FIVE) {
                tempCardPoint = 5;
            }
            if(cardValue(playerHand->card) == SIX) {
                tempCardPoint = 6;
            }
            if(cardValue(playerHand->card) == SEVEN) {
                tempCardPoint = 7;
            }
            if(cardValue(playerHand->card) == EIGHT) {
                tempCardPoint = 8;
            }
            if(cardValue(playerHand->card) == NINE) {
                tempCardPoint = 9;
            }
            if(cardValue(playerHand->card) == ADVANCE) {
                tempCardPoint = 10;
            }
            if(cardValue(playerHand->card) == BACKWARDS) {
                tempCardPoint = 11;
            }
            if(cardValue(playerHand->card) == CONTINUE) {
                tempCardPoint = 12;
            }
            if(cardValue(playerHand->card) == DECLARE) {
                tempCardPoint = 13;
            }
            if(cardValue(playerHand->card) == E) {
                tempCardPoint = 14;
            }
            if(cardValue(playerHand->card) == F) {
                tempCardPoint = 15;
            }
            pointCounter = pointCounter + tempCardPoint;;
            playerHand = playerHand->next;
        }
        game->playerInfo[playerCounter].points = pointCounter;
        playerCounter++;
    }
}


static void freeEntireList(cardList list) {
    if(list->head == NULL) {
        // do nothing
    } else {
        cardNode curr = list->head;
        cardNode prev = curr;
        while(curr->next != NULL) {
            destroyCard(prev->card);
            free(prev);
            prev = curr;
            curr = curr->next;
        }
    }
    free(list);
}



static int listSize(cardList list){
    int counter = 0;
    if (list->head == NULL) {
        // do nothing
    } else {
        cardNode curr = list->head;
        counter++;
        while(curr->next != NULL) {
            curr = curr->next;
            counter++;
        }
    }
    return counter;
}


static void putCardTopOfList(cardList list, Card card){
    cardNode new = calloc(1, sizeof(cardnode));
    new->card = card;
    new->next = list->head;
    list->head = new;
}


static void putCardEndOfList(cardList list, Card card) {
    cardNode new = calloc(1, sizeof(cardnode));
    new->card = card;
    new->next = NULL;
    if(list->head == NULL) {
        list->next = new;
    } else {
        cardNode curr = list->head;
        while(curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new;
    }
}


// records the moves made by players for all eternity (until they're freed)
static void recordInHistory(Game game, playerMove move) {
    assert(game->history->head != NULL);
    historyNode curr = game->history->head;
    while(curr->next != NULL) {
        curr = curr->next;
    }
    if(curr->turnNumber != game->turnNumber) {
        historyNode newTurnHistory = calloc(1, sizeof(historynode));
        newTurnHistory->turnNumber = game->turnNumber;
        newTurnHistory->playerNumber = game->currentPlayer;
        newTurnHistory->next = NULL;
        curr = newTurnHistory;
    } else {
        // do nothing
    }


    moveNode newMove = calloc(1, sizeof(movenode));
    newMove->move = move;
    newMove->next = NULL;

    if(curr->moves->head == NULL) {
        curr->moves->head = newMove;
    } else {
        moveNode currMove = curr->moves->head;
        while(currMove->next != NULL) {
            currMove = currMove->next;
        }
        currMove->next = newMove;
    }


}



// this one is going to be rather painful (look at the previous function,
// to see how convoluted setting up a history list is)
static void freeHistoryList(historyList list){
    if(list->head == NULL) {
        // do nothing
    } else {
        historyNode curr = list->head;
        historyNode prev = list->head;
        while(curr->next != NULL) {
            freeMoveList(prev->moves);
            free(prev);
            prev = curr;
            curr = curr->next;
        }
        free(curr);
        free(list);
    }

}


static void freeMoveList(moveList list) {
    moveNode curr = list->head;
    moveNode prev = list->head;
    while(curr->next != NULL) {
        free(prev);
        prev = curr;
        curr = curr->next;
    }
    free(curr);
    free(list);
}


static void drawFourCards(Game game) {
    while(game->discard->head != NULL) {

    }

}


static void drawTwoCards(Game game);


static int colorsMatch(Card first, Card second);

static void flipDiscardIntoDraw(Game game) {
    assert(game->draw->next == NULL);
    cardNode curr = game->discard->next;
    while(curr->next != NULL) {
        putCardEndOfList(game->draw, curr->card);
        curr = curr->next;
    }

}


static Card getNthCard(cardList list, int n){
    cardNode curr = list->head;
    int counter = 0;
    if(list->head == 0) {
        // do nothing
    }
    while(counter != n && curr->next != NULL) {
        curr = curr->next;
        counter++;
    }
    assert(curr->card != NULL);
    return curr->card;

}

static int cardsMatch(Card first, Card second) {
    int match = FALSE;
    if(cardValue(first) == cardValue(second)) {
        match = TRUE;
    } else if(cardSuit(first) == cardSuit(second)) {
        match = TRUE;
    } else if(cardColor(first) == cardColor(second)) {
        match = TRUE;
    }
    return match;
}

static int checkDrawTwoNonsense(Game game, playerMove move) {
    [][][]
}

static int checkPreviousMoves(Game game, playerMove move) {
    [][][]
}
