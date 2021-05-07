#include <stdlib.h>
#include <assert.h>
#include "map.h"

=============================
static MapKeyElement copyInt(MapKeyElement key_element)
{
    if (key_element == NULL) {
        return NULL;
    }
    int *newInt = malloc(sizeof(int));
    if (newInt == NULL)
    {
        return NULL;
    }
    *newInt = *(int *) key_element;
    return newInt;
}

static void freeInt(MapKeyElement key_element)
{
    free(key_element);
}

static MapDataElement copyTournament(MapDataElement data_element)
{
    if (data_element == NULL)
    {
        return NULL;
    }
    tournament *newTournament = malloc(sizeof(tournament));
    if (tournament == NULL)
    {
        return NULL;
    }
    *newTournament = *(tournament *) data_element;
    return newTournament;
}

static void freeTournament(MapDataElement data_element)
{
    free(data_element);
}

static int compareInt(MapKeyElement key_element1, MapKeyElement key_element2)
{
    assert(key_element1 != NULL && key_element2 != NULL);
    int key1 = *(* int) key_element1;
    int key2 = *(* int) key_element2;
    return (key1 - key2);
}
=============================================



typedef enum PlayerStatus_t
{
    ACTIVE,
    REMOVED
} PlayerStatus;

struct chess_system
        {
    Map tournament;
    Map players;
};

struct tournament
{
    int tournament_id;
    const char* tournament_location;
    int max_games_per_player;
    int winner;
    Map games;
};

struct game
{
    int game_id;
    int player1_id;
    int player2_id;
    Winner game_winner;
    int play_time;
};

struct player
{
    int player_id;
    int num_of_games;
    int num_of_victories;
    int num_of_losses;
    int num_of_draws;
    PlayerStatus player_status;
};

ChessSystem chessCreate()
{
    Map chess_system = mapCreate(copyTournament, copyInt, freeTournament, freeInt, compareInt);
    return chess_system;
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    if (chess == NULL || tournament_location == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id < 0)
    {
        return CHESS_INVALID_ID;
    }
    if (!location_is_valid)
    {
        return CHESS_INVALID_LOCATION;
    }
    if (mapContains(chess, tournament_id))
    {
        return CHESS_TOURNAMENT_ALREADY_EXIST;
    }
    *tournament new_tournament= malloc(sizeof(*new_tournament));
    if (new_tournament == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    new_tournament->tournament_id = tournament_id;
    new_tournament->tournament_location = tournament_location;
    new_tournament->max_games_per_player = max_games_per_player;
    new_tournament->winner = NULL;
    new_tournament->games = mapCreate(copyGame, copyInt, freeGame, freeInt, compareInt);

    if (mapPut(chess, tournament_id, new_tournament) == MAP_SUCCESS);
    {
        return CHESS_SUCCESS;
    }
    return CHESS_SUCCESS;
}