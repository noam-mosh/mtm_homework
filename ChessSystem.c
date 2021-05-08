#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "map.h"
#include "chessSystem.h"

#define UNDEFINED_WINNER -1;
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



enum PlayerStatus_t
{
    ACTIVE,
    REMOVED
};

struct chess_system{
    Map tournaments;
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

struct game_id
{
    int player1_id;
    int player2_id;
};

struct game
{
    game_id game_id;
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

static bool compareGameId(game_id game_id1, game_id game_id1)
{
    if(game_id1.player1_id != game_id2.player1_id && game_id1.player1_id != game_id2.player2_id)
    {
        return false;
    }
    return game_id1.player2_id == game_id2.player1_id || game_id1.player2_id == game_id2.player2_id
}

ChessSystem chessCreate()
{
    ChessSystem chess_system = malloc(sizeof(chess_system));
    if (chess_system == NULL)
    {
        return NULL;
    }
    Map tournaments = mapCreate(copyTournament, copyInt, freeTournament, freeInt, compareInt);
    if (tournaments == NULL)
    {
        free(chess_system);
        return NULL;
    }
    Map players = mapCreate(copyPlayer, copyInt, freePlayer, freeInt, compareInt);
    if (players == NULL)
    {
        mapDestroy(tournaments);
        free(chess_system);
        return NULL;
    }
    chess_system->tournaments = tournaments;
    chess_system->players = players;
    return chess_system;
}

static bool location_is_valid(const char* tournament_location)
{
    if ()
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
    if (!location_is_valid(tournament_location))
    {
        return CHESS_INVALID_LOCATION;
    }
    if (mapContains(chess, tournament_id))
    {
        return CHESS_TOURNAMENT_ALREADY_EXIST;
    }
    *tournament new_tournament= malloc(sizeof(new_tournament));
    if (new_tournament == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    new_tournament->tournament_id = tournament_id;
    new_tournament->tournament_location = tournament_location;
    new_tournament->max_games_per_player = max_games_per_player;
    new_tournament->winner = UNDEFINED_WINNER;
    new_tournament->games = mapCreate(copyGame, copyInt, freeGame, freeInt, compareGameId);
//TODO: copy game + player free game functions
    if (mapPut(chess->tournaments, tournament_id, new_tournament) == MAP_OUT_OF_MEMORY);
    {
        mapDestroy(new_tournament->games);
        free(new_tournament);
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}

static ChessResult gameParamsValid(ChessSystem chess, int tournament_id, int first_player,
                 int second_player, Winner winner, int play_time)
{
    if (tournament_id < 0 || first_player < 0 || second_player < 0)
    {
        return CHESS_INVALID_ID;
    }
    if (first_player = second_player || mapContains(chess->players, first_player) ||
            mapContains(chess->players, second_player))
    {
        return CHESS_INVALID_ID;
    }
    if (winner != first_player && winner != second_player)
    {
        return CHESS_INVALID_ID;
    }
    if (play_time < 0)
    {
        return CHESS_INVALID_PLAY_TIME;
    }
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (!mapContains(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    assert();
    if (gameParamsValid(chess, tournament_id, first_player, second_player, winner, play_time) != CHESS_SUCCESS)
    {
        return gameParamsValid
    }
    // TODO: fix game id struct
    game_id current_game_id = malloc(sizeof(current_game_id));
    if(current_game_id == NULL)
    {
        return CHESS_OUT_OF_MEMORY
    }
    if (mapContains(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_ALREADY_EXIST;
    }
    *tournament new_tournament= malloc(sizeof(new_tournament));
    if (new_tournament == NULL)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    new_tournament->tournament_id = tournament_id;
    new_tournament->tournament_location = tournament_location;
    new_tournament->max_games_per_player = max_games_per_player;
    new_tournament->winner = UNDEFINED_WINNER;
    new_tournament->games = mapCreate(copyGame, copyInt, freeGame, freeInt, compareInt);
}