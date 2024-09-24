#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // For usleep
#include <string.h>  // For strcmp
#include <stdbool.h>  // For boolean data type

#define NUM_CARDS 52  // Define a constant for the number of cards in a deck
#define BALANCE_FILE "balance.txt" // File to store player's balance

// Define arrays for card suits and ranks, and their corresponding values in Blackjack
const char *suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};
int values[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11}; 

// Struct definition remains the same
typedef struct {
    int player_hand_value;
    const char *actions[10]; // Using pointers to strings, 10 possible dealer up-cards
} BasicStrategyRow;

// Hard hands (regular hands)
BasicStrategyRow hard_strategy[] = {
    { 2,  {"h", "h", "h", "h", "h", "h", "h", "h", "h", "h"} },  // Player hand value 2
    { 3,  {"h", "h", "h", "h", "h", "h", "h", "h", "h", "h"} },  // Player hand value 3
    { 4,  {"h", "h", "h", "h", "h", "h", "h", "h", "h", "h"} },  // Player hand value 4
    { 5,  {"h", "h", "h", "h", "h", "h", "h", "h", "h", "h"} },  // Player hand value 5
    { 6,  {"h", "h", "h", "h", "h", "h", "h", "h", "h", "h"} },  // Player hand value 6
    { 7,  {"h", "h", "h", "h", "h", "h", "h", "h", "h", "h"} },  // Player hand value 7
    { 8,  {"h", "h", "h", "h", "h", "h", "h", "h", "h", "h"} },  // Player hand value 8
    { 9,  {"h", "d", "d", "d", "d", "d", "h", "h", "h", "h"} },  // Player hand value 9
    { 10, {"d", "d", "d", "d", "d", "d", "d", "d", "h", "h"} },  // Player hand value 10
    { 11, {"d", "d", "d", "d", "d", "d", "d", "d", "d", "h"} },  // Player hand value 11
    { 12, {"h", "h", "s", "s", "s", "h", "h", "h", "h", "h"} },  // Player hand value 12
    { 13, {"s", "s", "s", "s", "s", "h", "h", "h", "h", "h"} },  // Player hand value 13
    { 14, {"s", "s", "s", "s", "s", "h", "h", "h", "h", "h"} },  // Player hand value 14
    { 15, {"s", "s", "s", "s", "s", "h", "h", "h", "h", "h"} },  // Player hand value 15
    { 16, {"s", "s", "s", "s", "s", "h", "h", "h", "h", "h"} },  // Player hand value 16
    { 17, {"s", "s", "s", "s", "s", "s", "s", "s", "s", "s"} },  // Player hand value 17+
};

// Soft hands (hands with an Ace)
BasicStrategyRow soft_strategy[] = {
    { 20, {"s", "s", "s", "s", "s", "s", "s", "s", "s", "s"} },  // A9 (Ace with 9)
    { 19, {"s", "s", "s", "s", "s", "s", "s", "s", "s", "s"} },  // A8-A10 (Ace with 8)
    { 18, {"s", "d", "d", "d", "d", "s", "s", "h", "h", "h"} },  // A7 (total value = 18)
    { 17, {"h", "d", "d", "d", "d", "h", "h", "h", "h", "h"} },  // A6 
    { 16, {"h", "h", "d", "d", "d", "h", "h", "h", "h", "h"} },  // A5
    { 15, {"h", "h", "d", "d", "d", "h", "h", "h", "h", "h"} },  // A4
    { 14, {"h", "h", "h", "d", "d", "h", "h", "h", "h", "h"} },  // A3
    { 13, {"h", "h", "h", "d", "d", "h", "h", "h", "h", "h"} },  // A2
};

// Pairs (two of the same card)
BasicStrategyRow pair_strategy[] = {
    { 22, {"sp", "sp", "sp", "sp", "sp", "sp", "sp", "sp", "sp", "sp"} },  // AA (Always split)
    { 20, {"s", "s", "s", "s", "s", "s", "s", "s", "s", "s"} },  // 10,10 (Always stand)
    { 18, {"sp", "sp", "sp", "sp", "sp", "s", "sp", "sp", "s", "s"} },  // 9,9 (Split except vs. 7, 10, Ace)
    { 16, {"sp", "sp", "sp", "sp", "sp", "sp", "sp", "sp", "sp", "sp"} },  // 8,8 (Always split)
    { 14, {"h", "sp", "sp", "sp", "sp", "sp", "h", "h", "h", "h"} },  // 7,7 (Split until 9, otherwise hit)
    { 12, {"sp", "sp", "sp", "sp", "sp", "h", "h", "h", "h", "h"} },  // 6,6 (Split until 8, otherwise hit)
    { 10, {"d", "d", "d", "d", "d", "d", "d", "d", "h", "h"} },  // 5,5 (Double except vs. 10, Ace)
    { 8,  {"h", "h", "h", "h", "h", "h", "h", "h", "h", "h"} },  // 4,4 (Always hit)
    { 6,  {"h", "h", "sp", "sp", "sp", "sp", "h", "h", "h", "h"} },  // 3,3 (Split vs dealer 4-7, otherwise hit)
    { 4,  {"h", "h", "sp", "sp", "sp", "sp", "h", "h", "h", "h"} },  // 2,2 (Split vs dealer 4-7, otherwise hit)
};

int amount_plays = 0; // Initialize the amount of total plays to 0
int amount_of_correct_plays = 0; // Initialize the amount of correct plays to 0
int balance = 500; // Player's initial balance

// Define a structure to represent a card with rank, suit, and value
typedef struct {
    const char *rank;
    const char *suit;
    int value;
} Card;

// Function prototypes
void shuffle_deck(Card deck[]);
int calculate_hand_value(Card hand[], int num_cards);
void print_hand(Card hand[], int num_cards);
void play_hand(Card deck[], int *card_index, Card hand[], int *card_count, int *bet, int split_occurred);
void play_blackjack(); // Main Blackjack game function
void basic_strategy_calculation(Card player_hand[], int num_cards, int card_count, Card dealer_hand[], char choice[], char *result_strategy);
// Shuffle the deck of cards
void shuffle_deck(Card deck[]) {
    for (int i = 0; i < NUM_CARDS; i++) {
        int rand_idx = rand() % NUM_CARDS;
        Card temp = deck[i];
        deck[i] = deck[rand_idx];
        deck[rand_idx] = temp;
    }
}

// Calculate the total value of a hand of cards
int calculate_hand_value(Card hand[], int num_cards) {
    int total_value = 0;
    int num_aces = 0;

    for (int i = 0; i < num_cards; i++) {
        total_value += hand[i].value; // Add the value of the card to the total
        if (hand[i].value == 11) {    // Check if the card is an Ace
            num_aces++;               // Increment the number of Aces in the hand
        }
    }

    while (total_value > 21 && num_aces > 0) {
        total_value -= 10;
        num_aces--;
    }

    return total_value;
}

// Print the cards in a hand and their total value
void print_hand(Card hand[], int num_cards) {
    for (int i = 0; i < num_cards; i++) {
        printf("%s of %s\n", hand[i].rank, hand[i].suit);
    }
    printf("Total value: %d\n\n", calculate_hand_value(hand, num_cards));
    usleep(1000000);  // Pause for 1 second before continuing
}

// Play a single hand of Blackjack (hit, stand, double)
void play_hand(Card deck[], int *card_index, Card hand[], int *card_count, int *bet, int split_occurred) {
    char choice[5];
    int initial_hand_value = calculate_hand_value(hand, *card_count);
    char result_strategy[200];

    basic_strategy_calculation(hand, *card_count, *card_count, hand, choice, result_strategy);
    
    // if the player splitted with aces, they can only draw one card for each hand and have no choice
    if (split_occurred == 1 && initial_hand_value == 22) {
        printf("You have split with Aces. You can only draw one card for each hand.\n");
        usleep(1000000);  // Pause for 1 second before continuing
        printf("\nDo you want to hit (h), stand (s), or double (d)? ");
        // call the basic strategy function and store the result in the result_strategy variable
        hand[(*card_count)++] = deck[--(*card_index)];
        printf("\nYour hand:\n");
        print_hand(hand, *card_count);

        // Update the player's hand value after adding the new card
        int current_hand_value = calculate_hand_value(hand, *card_count);

        // Call the basic strategy function with the updated hand value
        basic_strategy_calculation(hand, *card_count, current_hand_value, hand, choice, result_strategy);
        printf("-------------\n%s \n-------------\n", result_strategy);

        if (calculate_hand_value(hand, *card_count) > 21) {
            printf("You bust! Dealer wins this hand.\n");
            balance -= *bet;  // Decrease player's balance by the bet amount
            return;
        }
    // if the player has not splitted with aces, they can choose to hit, stand, or double
    } else{
        while (1) {
            printf("\nDo you want to hit (h), stand (s), or double (d)? ");
            scanf("%s", choice);
            
            if (strcmp(choice, "h") == 0) {
                // call the basic strategy function and store the result in the result_strategy variable
                hand[(*card_count)++] = deck[--(*card_index)];
                printf("\nYour hand:\n");
                print_hand(hand, *card_count);

                // Update the player's hand value after adding the new card
                int current_hand_value = calculate_hand_value(hand, *card_count);

                // Call the basic strategy function with the updated hand value
                basic_strategy_calculation(hand, *card_count, current_hand_value, hand, choice, result_strategy);
                printf("-------------\n%s \n-------------\n", result_strategy);

                if (calculate_hand_value(hand, *card_count) > 21) {
                    printf("You bust! Dealer wins this hand.\n");
                    balance -= *bet;  // Decrease player's balance by the bet amount
                    return;
                }
            } else if (strcmp(choice, "d") == 0 && *bet * 2 <= balance && *card_count == 2 && split_occurred == 0) {
                printf("You are currently betting: %d\n", *bet * 2);
                usleep(1000000);  // Pause for 1 second before continuing
                
                // Update the player's hand value after adding the new card
                int current_hand_value = calculate_hand_value(hand, *card_count);
                // Call the basic strategy function with the updated hand value
                basic_strategy_calculation(hand, *card_count, current_hand_value, hand, choice, result_strategy);
                hand[(*card_count)++] = deck[--(*card_index)];
                printf("\nYour hand:\n");
                print_hand(hand, *card_count);
                printf("-------------\n%s \n-------------\n", result_strategy);

                if (calculate_hand_value(hand, *card_count) > 21) {
                    printf("You bust! Dealer wins this hand.\n");
                    balance -= *bet * 2;  // Decrease player's balance by double the bet amount
                    return;
                }
                *bet *= 2;
                break;
            } else if (strcmp(choice, "s") == 0) {
                // Update the player's hand value after adding the new card
                int current_hand_value = calculate_hand_value(hand, *card_count);

                // Call the basic strategy function with the updated hand value
                basic_strategy_calculation(hand, *card_count, current_hand_value, hand, choice, result_strategy);
                printf("-------------\n%s \n-------------\n", result_strategy);
                break;
            } else {
                // if the player chooses to double down but does not have enough balance give special message
                if(strcmp(choice, "d") == 0 && balance < *bet * 2) {
                    printf("You don't have enough balance to double down. Please choose another option.\n");

                } else{
                    printf("Invalid choice! Please enter 'h', 'd', or 's'.\n");
                }
            }
        }
    }


}

// Main Blackjack game function
void play_blackjack() {    
    int bet = 0;
    char result_strategy[200];
    while (1) {
        printf("Your current balance: %d \n", balance);
        printf("Enter your bet amount: ");

        if (scanf("%d", &bet) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a valid integer.\n \n------------------------------------\n");
            continue;
        }

        if (bet > balance) {
            printf("You don't have enough balance to place this bet. Please try again.\n");
            continue;
        } else if (bet <= 0) {
            printf("Invalid bet amount. Please enter a positive value.\n");
            continue;
        }

        break;
    }
    // Initialize the deck of cards
    Card deck[NUM_CARDS];
    int card_index = 0;
    // loop through the suits and ranks to create the deck of cards
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            deck[card_index].rank = ranks[j];
            deck[card_index].suit = suits[i];
            deck[card_index].value = values[j];
            card_index++;
        }
    }

    shuffle_deck(deck);

    Card player_hand[10];
    Card dealer_hand[10];
    Card split_hand[10]; // Declare split_hand here
    int player_card_count = 0; // Initialize player_card_count to 0, to keep track of the number of cards in the player's hand
    int dealer_card_count = 0; // Initialize dealer_card_count to 0, to keep track of the number of cards in the dealer's hand
    int split_card_count = 0;  // Initialize split_card_count to 0, to keep track of the number of cards in the split hand
    int split_occurred = 0;  // Flag to check if a split has occurred

    player_hand[player_card_count++] = deck[--card_index];
    player_hand[player_card_count++] = deck[--card_index];
    dealer_hand[dealer_card_count++] = deck[--card_index];
    dealer_hand[dealer_card_count++] = deck[--card_index];

    printf("\nYour hand:\n");
    print_hand(player_hand, player_card_count);


    usleep(1000000);  // Pause for 1 second before continuing
    printf("\nDealer's hand:\n");
    printf("%s \n", dealer_hand[0].rank);  // Only show one card
    usleep(1000000);  // Pause for 1 second before continuing

    // Check for Blackjack
    int player_total = calculate_hand_value(player_hand, player_card_count);
    int dealer_total = calculate_hand_value(dealer_hand, dealer_card_count);


    if (player_total == 21 && player_card_count == 2 && dealer_total == 21 && dealer_card_count == 2) {
        printf("Both you and the dealer have Blackjack! It's a push.\n");
        return;
    } else if (dealer_total == 21 && dealer_card_count == 2) {
        // Print dealers second card
        print_hand(dealer_hand, dealer_card_count);
        printf("Dealer has Blackjack! Dealer wins.\n");
        balance -= bet;
        return;
    } else if(player_total == 21 && player_card_count == 2) {
        printf("You have Blackjack! You win!\n");
        balance += bet * 1.5;  // Blackjack pays 3:2
        return;
    }

    char choice[4];
    while (1) {
        printf("\nDo you want to hit (h), stand (s), double (d), or split (sp)? ");
        scanf("%s", choice);

        if (strcmp(choice, "h") == 0) {
            // call the basic strategy function and store the result in the result_strategy variable
            basic_strategy_calculation(player_hand, player_card_count, player_card_count, dealer_hand, choice, result_strategy);

            player_hand[player_card_count++] = deck[--card_index];
            printf("\nYour hand:\n");
            print_hand(player_hand, player_card_count);

            
            printf("-------------\n%s \n-------------\n", result_strategy);
            if (calculate_hand_value(player_hand, player_card_count) > 21) {
                printf("You bust! Dealer wins.\n");
                balance -= bet;  // Decrease player's balance by the bet amount
                return;
            }
        // making sure that the player can only double down if they have two cards in their hand, and they have enough balance to double their bet, and no split
        } else if (strcmp(choice, "d") == 0 && bet * 2 <= balance && player_card_count == 2 && split_occurred == 0) {
            printf("Your bet is now: %d\n", bet * 2);
            usleep(1000000);  // Pause for 1 second before continuing
            // call the basic strategy function and store the result in the result_strategy variable
            basic_strategy_calculation(player_hand, player_card_count, player_card_count, dealer_hand, choice, result_strategy);
            player_hand[player_card_count++] = deck[--card_index];
            // print the result of basic strategy
            printf("-------------\n%s \n-------------\n", result_strategy);
            printf("\nYour hand:\n");
            print_hand(player_hand, player_card_count);

            
            usleep(1000000);  // Pause for 1 second before continuing

            if (calculate_hand_value(player_hand, player_card_count) > 21) {
                printf("You bust! Dealer wins.\n");
                balance -= bet * 2;  // Decrease player's balance by double the bet amount
                return;
            }
            bet *= 2;
            break;
        } else if (strcmp(choice, "s") == 0) {
            // Calculate the current hand value
            int current_hand_value = calculate_hand_value(player_hand, player_card_count);
            // Call the basic strategy function with the current hand value
            basic_strategy_calculation(player_hand, player_card_count, current_hand_value, dealer_hand, choice, result_strategy);
            printf("-------------\n%s \n-------------\n", result_strategy);
            break;
        } else if (strcmp(choice, "sp") == 0 && player_card_count == 2 && player_hand[0].value == player_hand[1].value && bet * 2 <= balance) {
            printf("You chose to split!\n");
            printf("You are currently betting: %d\n", bet * 2);   
            int current_hand_value = calculate_hand_value(player_hand, player_card_count);
            // Call the basic strategy function with the current hand value
            basic_strategy_calculation(player_hand, player_card_count, current_hand_value, dealer_hand, choice, result_strategy);
            printf("-------------\n%s \n-------------\n", result_strategy);
            split_occurred = 1;
            // Create a second hand for the split
            split_hand[split_card_count++] = player_hand[1];  // Move second card to split hand
            player_card_count = 1;  // Keep only the first card in the original hand
            

            // Deal a card to the first hand only (player finishes playing the first hand before moving to the second)
            printf("\nFirst hand:\n");
            player_hand[player_card_count++] = deck[--card_index];  // Deal to the first hand
            print_hand(player_hand, player_card_count);
            play_hand(deck, &card_index, player_hand, &player_card_count, &bet, split_occurred);  // Play the first hand

            // Deal a card to the second hand and play it after the first hand is finished
            printf("\nSecond hand:\n");
            split_hand[split_card_count++] = deck[--card_index];  // Deal to the second hand
            print_hand(split_hand, split_card_count);
            play_hand(deck, &card_index, split_hand, &split_card_count, &bet, split_occurred);  // Play the second hand, passing deck, card_index, split_hand, split_card_count

        //    balance -= bet;  // Deduct additional bet for the split
            break;
        } else {

            if(strcmp(choice, "d") == 0 && balance < bet * 2) {
                printf("You don't have enough balance to double down. Please choose another option.\n");
            // if the player tries to split without having two cards of the same rank
            } else if(strcmp(choice, "sp") == 0 && player_hand[0].value != player_hand[1].value) {
                printf("You can only split with two cards of the same rank. Please choose another option.\n");
            // if the player tries to split when having more than 2 cards in their hand
            } else if(strcmp(choice, "sp") == 0 && player_card_count > 2) {
                printf("You can only split with two cards in your hand. Please choose another option.\n");
            // if the player tries to split but doesn't have enough balance
            } else if(strcmp(choice, "sp") == 0 && bet * 2 > balance) {
                printf("You don't have enough balance to split. Please choose another option.\n");
            // if the player does any other illegal move
            } else{
                printf("Invalid choice! Please enter 'h', 'd', 's', or 'sp'.\n");
            }
        }
    }

    printf("\nDealer's hand:\n");
    print_hand(dealer_hand, dealer_card_count);
    while (calculate_hand_value(dealer_hand, dealer_card_count) < 17) {
        dealer_hand[dealer_card_count++] = deck[--card_index];
        printf("\nDealer hits:\n");
        print_hand(dealer_hand, dealer_card_count);

        if (calculate_hand_value(dealer_hand, dealer_card_count) > 21) {
            printf("Dealer busts! You win!\n");
            balance += bet;
            return;
        }
    }

    // Compare player's two hands (if split) with the dealer's hand
    int player_total_first_hand = calculate_hand_value(player_hand, player_card_count);
    int player_total_second_hand = calculate_hand_value(split_hand, split_card_count);
    dealer_total = calculate_hand_value(dealer_hand, dealer_card_count);

    printf("\nFinal Results:\n");

    // Check if a split occurred
    if (split_card_count == 0) {   
        // No split occurred; only one hand to evaluate
        if(player_total_first_hand > 21) {
            printf("You bust! Dealer wins.\n");
            balance -= bet;
        }
        else if(dealer_total > 21) {
            printf("Dealer busts! You win!\n");
            balance += bet;
        }
        else if (player_total_first_hand > dealer_total) {
        printf("You win!\n");
        balance += bet;
        } else if (player_total_first_hand < dealer_total) {
            printf("Dealer wins!\n");
            balance -= bet;
        } else {
            printf("Same value: It's a push!\n");
        }
        printf("Your hand value: %d\n", player_total_first_hand);
    } else {
        // Split occurred; evaluate each hand separately
        // First hand results
        if (player_total_first_hand > dealer_total) {
        printf("First hand: You win!\n");
        balance += bet;
        } else if (player_total_first_hand < dealer_total) {
            printf("First hand: Dealer wins!\n");
            balance -= bet;
        } else {
            printf("First hand: It's a push!\n");
        }
        printf("First hand value: %d\n", player_total_first_hand);
    }
    
    printf("Dealer's hand value: %d\n", dealer_total);


    // Second hand results
    if(split_card_count > 0) {
        printf("\nSecond hand value: %d\n", player_total_second_hand);
        printf("Dealer's hand value: %d\n", dealer_total);

        if(player_total_second_hand > 21) {
            printf("Second hand: You bust! Dealer wins.\n");
            balance -= bet; // deduct the bet amount, for the second hand
        } else if(dealer_total > 21) {
            printf("Second hand: Dealer busts! You win!\n");
            balance += bet; // add the bet amount, for the second hand
        }
        else if (player_total_second_hand > dealer_total) {
            printf("Second hand: You win!\n");
            balance += bet; // add the bet amount, for the second hand
        } else if (player_total_second_hand < dealer_total) {
            printf("Second hand: Dealer wins!\n");
            balance -= bet; // deduct the bet amount, for the second hand
        } else {
            printf("Second hand: It's a push!\n");
        }
    }
};
    

// Basic strategy calculation function

void basic_strategy_calculation(Card player_hand[], int num_cards, int card_count, Card dealer_hand[], char choice[], char *result_strategy) {
    int total_value = 0;
    int num_aces = 0;
    int dealer_upcard_value = dealer_hand[0].value; // Dealer's upcard value
    const char *correct_choice; // Correct choice based on basic strategy
    bool is_pair = (num_cards == 2 && player_hand[0].value == player_hand[1].value);
    bool is_soft = player_hand[0].value == 11 || player_hand[1].value == 11;

    // checking if the player has a soft hand
    for (int i = 0; i < num_cards; i++) {
        total_value += player_hand[i].value; // Add the value of the card to the total
        if (player_hand[i].value == 11) {    // Check if the card is an Ace
            num_aces++;               // Increment the number of Aces in the hand
        }
    }

    while (total_value > 21 && num_aces > 0) {
        total_value -= 10;
        num_aces--;
    }
    if (is_pair) { // Check if the player has a pair
        for (int i = 0; i < sizeof(pair_strategy) / sizeof(pair_strategy[0]); i++) { // Loop through the pair strategy matrix
            if (total_value == pair_strategy[i].player_hand_value) { // Check if the player's hand value matches the strategy row
                correct_choice = pair_strategy[i].actions[dealer_upcard_value - 2]; // set the correct choice based on the strategy matrix
                if(is_soft && strcmp(choice, correct_choice) == 0) {
                    // add to the correct choice file
                    amount_plays++;
                    amount_of_correct_plays++;
                    sprintf(result_strategy, "\nCorrect choice! When you have a pair of Aces, you should always %s.\n", correct_choice); 
                } 
                else if(is_soft && strcmp(choice, correct_choice) != 0){
                    amount_plays++;
                    sprintf(result_strategy, "\nIncorrect choice. When you have a pair of Aces, you should always %s.\n", correct_choice);
                }
                else if (strcmp(choice, correct_choice) == 0) {
                    amount_plays++;
                    amount_of_correct_plays++;
                    sprintf(result_strategy, "\nCorrect choice! When you have a pair of %s against a %d, you should %s.\n", player_hand[0].rank, dealer_upcard_value, correct_choice);
                } else {
                    amount_plays++;
                    sprintf(result_strategy, "\nIncorrect choice. When you have a pair of %s against a %d, the correct move is %s.\n", player_hand[0].rank, dealer_upcard_value, correct_choice);
                }
                return;
            }
        }
    } // if the player has a soft hand (hand with an Ace), use the basic strategy matrix to determine the correct move against the dealers card
    else if (is_soft) { // Check if the player has an Ace in the hand
        for (int i = 0; i < sizeof(soft_strategy) / sizeof(soft_strategy[0]); i++) { // Loop through the soft strategy matrix
            if (total_value == soft_strategy[i].player_hand_value) { // Check if the player's hand value matches the strategy row
                if (dealer_upcard_value == 11) {                    // Check if the dealer's upcard is an Ace
                    correct_choice = soft_strategy[i].actions[9]; // set the correct choice based on the strategy matrix
                    if (strcmp(choice, correct_choice) == 0) {
                        amount_plays++;
                        amount_of_correct_plays++;
                        sprintf(result_strategy, "\nCorrect choice! When you have a soft %d against an Ace, you should %s.\n", total_value, correct_choice);
                    } else {
                        amount_plays++;
                        sprintf(result_strategy, "\nIncorrect choice. When you have a soft %d against an Ace, the correct move is %s.\n", total_value, correct_choice);
                    }
                    return;
                } else {
                    correct_choice = soft_strategy[i].actions[dealer_upcard_value - 2];
                    if (strcmp(choice, correct_choice) == 0) {
                        amount_plays++;
                        amount_of_correct_plays++;
                        sprintf(result_strategy, "\nCorrect choice! When you have a soft %d against a %d, you should %s.\n", total_value, dealer_upcard_value, correct_choice);
                    } else {
                        amount_plays++;
                        sprintf(result_strategy, "\nIncorrect choice. When you have a soft %d against a %d, the correct move is %s.\n", total_value, dealer_upcard_value, correct_choice);
                    }
                    return;
                }
            }
        }
    } // if the player has a pair
    
    // if the player has a hard hand (hand without an Ace)
    else { // For hard hands
    if (total_value >= 17) {
        // Use the strategy for 17+
        for (int i = 0; i < sizeof(hard_strategy) / sizeof(hard_strategy[0]); i++) {
            if (hard_strategy[i].player_hand_value == 17) {
                correct_choice = hard_strategy[i].actions[dealer_upcard_value - 2];
                if (strcmp(choice, correct_choice) == 0) {
                    amount_plays++;
                    amount_of_correct_plays++;
                    sprintf(result_strategy, "\nCorrect choice! When you have a hard %d against a %d, you should %s.\n", total_value, dealer_upcard_value, correct_choice);
                } else {
                    amount_plays++;
                    sprintf(result_strategy, "\nIncorrect choice. When you have a hard %d against a %d, the correct move is %s.\n", total_value, dealer_upcard_value, correct_choice);
                }
                return;
            }
        }
        } else {
            // For other hard hand values
            for (int i = 0; i < sizeof(hard_strategy) / sizeof(hard_strategy[0]); i++) {
                if (total_value == hard_strategy[i].player_hand_value) {
                    correct_choice = hard_strategy[i].actions[dealer_upcard_value - 2];
                    if (strcmp(choice, correct_choice) == 0) {
                        amount_plays++;
                        amount_of_correct_plays++;
                        sprintf(result_strategy, "\nCorrect choice! When you have a hard %d against a %d, you should %s.\n", total_value, dealer_upcard_value, correct_choice);
                    } else {
                        amount_plays++;
                        sprintf(result_strategy, "\nIncorrect choice. When you have a hard %d against a %d, the correct move is %s.\n", total_value, dealer_upcard_value, correct_choice);
                    }
                    return;
                }
            }
        }
    }
}







int main() {
    srand(time(0));

    printf("Welcome to Blackjack!\n");

    // Attempt to open the balance file for reading
    FILE *balance_file = fopen(BALANCE_FILE, "r");
    if (balance_file != NULL) {
        // If the file exists, read the balance from it
        if (fscanf(balance_file, "%d", &balance) != 1) {
            // If reading fails, set balance to default
            balance = 500;
            printf("Error reading balance. Starting with default balance of: %d\n", balance);
        }
        fclose(balance_file);
    } else {
        // If the file doesn't exist, start with default balance
        balance = 500;
        printf("Starting with default balance of: %d\n", balance);
    }

    // Optionally, ask the user if they want to reset their balance
    char reset_choice;
    printf("Do you want to reset your balance to 500? (y/n): ");
    scanf(" %c", &reset_choice);
    if (reset_choice == 'y' || reset_choice == 'Y') {
        balance = 500;
    }
    
    while (1) {
        play_blackjack();

        if (balance == 0) {
            printf("You dirty gambler, go home and think of your decisions\n");
            float percentage_correct = (float)amount_of_correct_plays / amount_plays * 100;
            printf("You made %d plays and %d of them were correct. That means that %.2f%% of your plays were correct.\n", amount_plays, amount_of_correct_plays, percentage_correct);
            break;
        } else {
            printf("Your new balance is: %d\n", balance);
            usleep(1000000);  // Pause for 1 second before continuing
        }

        char decision;
        while (1) {
            printf("\n------------------------------------\nWould you like to gamble again? (y/n): \n");
            scanf(" %c", &decision);
            // check if the input is valid
            if (decision == 'y' || decision == 'n') {
                break;
            } else {
                // if the input is invalid, prompt the user to enter 'y' or 'n'
                printf("Invalid input. Please enter 'y' or 'n'.\n");
                // clear the input buffer
                while (getchar() != '\n');
            }
        }

        if (decision == 'n') {
            // calculate the percentage of correct plays
            float percentage_correct = (float)amount_of_correct_plays / amount_plays * 100;
            printf("You made %d plays and %d of them were correct. That means that %.2f%% of your plays were correct.\n", amount_plays, amount_of_correct_plays, percentage_correct);
            printf("Thanks for playing Blackjack! Goodbye!\n");
            break;
        }     
    }
        // Before exiting, save the balance to the file
    balance_file = fopen(BALANCE_FILE, "w");
    if (balance_file != NULL) {
        fprintf(balance_file, "%d", balance);
        fclose(balance_file);
    } else {
        printf("Error: Unable to save balance to file.\n");
    }
    return 0;
}
