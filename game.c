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

typedef struct _history {
    
} history;


typedef struct _pile {
    Node head;
} pile;


typedef struct _node {
    Card value;
    Node next;
} node;


typedef struct _player {
    pile hand;
    int points;
} player;


typedef struct _game {
    int totalDeckSize;
    pile originalDeck;
    pile discard;
    pile draw;
    player playerInfo[4];
    int turnNumber;
    direction currentDirection;
    history history[4];
} game;

Game newGame(int deckSize, value values[], color colors[], suit suits[]){


    Game new = calloc(1, sizeof(game));
    // the total number of cards we are given at the beginning
    new->totalDeckSize = deckSize;




    // set up the draw pile
    new->draw.pileSize = deckSize-29;
    new->draw.firstValue = calloc(deckSize, sizeof(value));
    new->draw.firstColor = calloc(deckSize, sizeof(color));
    new->draw.firstSuit = calloc(deckSize, sizeof(suit));

    // assign the cards after 28 to the draw pile
    // because the first 28 (0 - 27) will be dealt out,
    // and one (28) for the discard pile
    int drawCounter = 29;
    while(counter < deckSize){
        new->draw.firstValue[drawCounter] = values[drawCounter];
        new->draw.firstColor[drawCounter] = colors[drawCounter];
        new->draw.firstSuit[drawCounter] = suits[drawCounter];
        drawCounter++;
    }

    // a copy of the original deck, for checking the number of things in it
    new->originalDeck.pileSize = deckSize;
    new->originalDeck.firstValue = calloc(deckSize, sizeof(value));
    new->originalDeck.firstColor = calloc(deckSize, sizeof(color));
    new->originalDeck.firstSuit = calloc(deckSize, sizeof(suit));

    // and assigning
    int originalDeckCounter = 0; // because the first 28 will be dealt out
    while(counter < deckSize){
        new->originalDeck.firstValue[originalDeckCounter] = values[originalDeckCounter];
        new->originalDeck.firstColor[originalDeckCounter] = colors[originalDeckCounter];
        new->originalDeck.firstSuit[originalDeckCounter] = suits[originalDeckCounter];
        originalDeckCounter++;
    }

    new->playerInfo = calloc(NUM_PLAYERS, sizeof(player));
    int playerCounter = 0;
    while(playerCounter < 4){
        new->playerInfo[playerCounter].hand.pileSize = 7;
        new->playerInfo[playerCounter].hand.firstValue = calloc(deckSize-3, sizeof(value));
        new->playerInfo[playerCounter].hand.firstColor = calloc(deckSize-3, sizeof(color));
        new->playerInfo[playerCounter].hand.firstSuit = calloc(deckSize-3, sizeof(suit));
        playerCounter++;
    }

    int dealCounter = 0;
    while(dealCounter < 28){
        new->playerInfo[dealCounter%4].hand.firstValue[(dealCounter - dealCounter%4)/4] = values[counter];
        new->playerInfo[dealCounter%4].hand.firstColor[(dealCounter - dealCounter%4)/4] = colors[counter];
        new->playerInfo[dealCounter%4].hand.firstSuits[(dealCounter - dealCounter%4)/4] = suits[counter];
        dealCounter++;
    }

    new->turnNumber = 0;
    new->currentDirection = CLOCKWISE;

    // set up the discard pile
    new->discard.pileSize = 1;
    new->discard.firstValue = calloc(deckSize, sizeof(value));
    new->discard.firstColor = calloc(deckSize, sizeof(color));
    new->discard.firstSuit = calloc(deckSize, sizeof(suit));

    // put the card after all the dealing in the discard pile
    new->draw.firstValue[0] = values[28];
    new->draw.firstColor[0] = colors[28];
    new->draw.firstSuit[0] = suits[28];

    return new;
}

void destroyGame(Game g){
    free(g->playerInfo[playerCounter].hand.firstValue);
    free(g->playerInfo[playerCounter].hand.firstColor);
    free(g->playerInfo[playerCounter].hand.firstSuit);

    free(g->draw.firstValue);
    free(g->draw.firstColor);
    free(g->draw.firstSuit);

    free(g->discard.firstValue);
    free(g->discard.firstColor);
    free(g->discard.firstSuit);

    free(g->originalDeck.firstValue[originalDeckCounter]);
    free(g->originalDeck.firstColor[originalDeckCounter]);
    free(g->originalDeck.firstSuit[originalDeckCounter]);

    free(g);
}

int numCards(Game g){
    return g->totalDeckSize;
}

int numOfSuit(Game game, suit suit){
    int counter = 0;
    int suitCounter = 0;
    while(counter < game->totalDeckSize){
        if(game->originalDeck.firstSuit[counter] = suit){
            suitCounter++;
            counter++;
        } else{
            counter++;
        }
    }
    return suitCounter;
}

int numOfColor(Game game, color color){
    int counter = 0;
    int colorCounter = 0;
    while(counter < game->totalDeckSize){
        if(game->originalDeck.firstColor[counter] = color){
            colorCounter++;
            counter++;
        } else{
            counter++;
        }
    }
    return colorCounter;
}

int numOfValue(Game game, value value){
    int counter = 0;
    int valueCounter = 0;
    while(counter < game->deckSize){
        if(game->originalDeck.firstValue[counter] = value){
            valueCounter++;
            counter++;
        } else{
            counter++;
        }
    }
    return valueCounter;
}

int currentPlayer(Game game){
    return game->turnNumber%4;
}

int currentTurn(Game game){
    return game->turnNumber;
}

// NOT to be called by players
int playerPoints(Game game, int player){
    return game->playerInfo[player].points;
}

direction playDirection(Game game){
    return game->direction;
}

int pastTurns(Game game){
    [][][];
}

int turnMoves(Game game, int n){
    [][][]
}

void playMove(Game game, playerMove move){
    assert(isValidMove(Game game, playerMove move) == TRUE);
    if(move.action == DRAW_CARD){
        Card topCard =
        assert(game->playerInfo[currentPlayer(game)]->pile != NULL);
        Node newNode = calloc(1,sizeof(node));
        assert(newNode != NULL);
        newNode->next = NULL;
        newNode->value = topCard;

        if(list->head == NULL) { //list is empty
            list->head = newNode;
        } else { //list contains elements
            Node curr = list->head;
            while(curr->next != NULL){ //find node before null
                curr = curr->next;
            }
            curr->next = newNode;
    }
}

        game->playerInfo[currentPlayer(game)].hand.firstValue =
    }

}

Card handCard(Game game, int n) {
    int counter = 0;
    Node curr = game->playerInfo[currentPlayer(game)]->hand->head->next;
    while(counter < n) {
        curr = curr->next;
    }
    Card card = curr->value;

    return card;
}

int playerCardCount(Game game, int player){
    int counter = 0;
    Node curr = game->playerInfo[player]->hand->head;
    while(curr->next != NULL){
        curr = curr->next;
        counter++;
    }
    return counter;
}

int handCardCount(Game game){
    int counter = 0;
    Node curr = game->playerInfo[currentPlayer(game)]->hand->head;
    while(curr->next != NULL){
        curr = curr->next;
        counter++;
    }
    return counter;
}
















}
