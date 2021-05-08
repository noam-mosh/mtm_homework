#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "map.h"
#include "chessSystem.h"

#define UNDEFINED_WINNER -1;
=============================

struct chess_system{
    Map tournaments;
    Map players;
};

typedef struct tournament
{
    int tournament_id;
    const char* tournament_location;
    int max_games_per_player;
    int winner;
    Map games;
} *Tournament;

typedef struct game_id
{
    int player1_id;
    int player2_id;
} *GameId;

typedef struct game
{
    game_id game_id;
    int player1_id;
    int player2_id;
    Winner game_winner;
    int play_time;
} *Game;

typedef struct player
{
    int player_id;
    int num_of_games;
    int num_of_victories;
    int num_of_losses;
    int num_of_draws;
    PlayerStatus player_status;
} *Player;

enum PlayerStatus_t
{
    ACTIVE,
    REMOVED
};

=============================================

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
    Tournament new_tournament = malloc(sizeof(new_tournament));
    if (new_tournament == NULL)
    {
        return NULL;
    }
    *new_tournament = *(Tournament) data_element;
    return new_tournament;
}

static void freeTournament(MapDataElement data_element)
{
    mapDestroy(data_element->games);
    free(data_element->tournament_location);
    free(data_element)
}

static int compareInt(MapKeyElement key_element1, MapKeyElement key_element2)
{
    assert(key_element1 != NULL && key_element2 != NULL);
    int key1 = *(* int) key_element1;
    int key2 = *(* int) key_element2;
    return (key1 - key2);
}

static MapDataElement copyGame(MapDataElement data_element)
{
    if (data_element == NULL)
    {
        return NULL;
    }
    Game newGame = malloc(sizeof(newGame));
    if (newGame == NULL)
    {
        return NULL;
    }
    *newGame = *(Game) data_element;
    return newGame;
}

static void freeGame(MapDataElement data_element)
{
    free(data_element);
}

static MapKeyElement copyGameId(MapKeyElement key_element)
{
    if (key_element == NULL)
    {
        return NULL;
    }
    GameId newGameId = malloc(sizeof(newGameId));
    if (newGameId == NULL)
    {
        return NULL;
    }
    *newGameId = *(GameId) key_element;
    return newGameId;
}

static void freeGameId(MapKeyElement key_element)
{
    free(key_element);
}

static int compareGameId(MapKeyElement key_element1, MapKeyElement key_element2)
{
    assert(key_element1 != NULL && key_element2 != NULL && key_element1 != key_element2);
    GameId game_id1 = *(GameId) key_element1;
    GameId game_id2 = *(GameId) key_element2;
    if(game_id1->player1_id != game_id2->player1_id && game_id1->player1_id != game_id2->player2_id)
    {
        return POSITIVE_INT;
    }
    if (game_id1->player2_id == game_id2->player1_id || game_id1->player2_id == game_id2->player2_id)
    {
        return 0;
    }
    return POSITIVE_INT;

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
    Tournament new_tournament= malloc(sizeof(new_tournament));
    if (new_tournament == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    char* tournament_location_copy = malloc(strlen(tournament_location) + 1);
    if (tournament_location_copy == NULL)
    {
        free(new_tournament);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    strcpy(tournament_location_copy, tournament_location);
    new_tournament->tournament_location = tournament_location_copy;
    new_tournament->tournament_id = tournament_id;
    new_tournament->max_games_per_player = max_games_per_player;
    new_tournament->winner = UNDEFINED_WINNER;
    new_tournament->games = mapCreate(copyGame, copyGameId, freeGame, freeGameId, compareGameId);
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
                 int second_player, Winner winner, int play_time, GameId game_id)
{
    if (tournament_id < 0 || first_player < 0 || second_player < 0)
    {
        return CHESS_INVALID_ID;
    }
    if (winner != FIRST_PLAYER && winner != SECOND_PLAYER && winner != DRAW)
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
    if (!mapContains(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    assert(chess->tournaments != NULL && tournament_id != NULL && !mapContains(chess->tournaments, tournament_id));
    Tournament current_tournament = mapGet(chess->tournaments, tournament_id);
    if (mapContains(current_tournament->games, current_game_id))
    {
        free(current_game_id);
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (play_time < 0)
    {
        free(current_game_id);
        return CHESS_INVALID_PLAY_TIME;
    }
    if (!player_game_count_valid(chess, current_tournamentm ,first_player)
        || !player_game_count_valid(chess, current_tournamentm ,second_player))
    {
        return CHESS_EXCEEDED_GAMES;
    }
    return CHESS_SUCCESS;
}

static bool player_game_count_valid(ChessSystem chess, Tournament tournament, int player_id)
{
    int max_games = tournament->max_games_per_player;
    if(chess->players == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    Player player = mapGet(chess->players, player_id);
    if(player == NULL)
    {
        return true;
    }
    if(player->num_of_games < max_games)
        return true;
    return false;
}

static ChessResult update_players_map(ChessSystem chess, int player_id, int victory, int lose, int draw)
{
    Player player = mapGet(chess->players, player_id);
    if(player == NULL)
    {
        Player player= malloc(sizeof(new_player));
        if (player == NULL)
        {
            return CHESS_OUT_OF_MEMORY;
        }
        player->player_id = player_id;
        player->num_of_games = 0;
        player->num_of_victories = 0;
        player->num_of_losses = 0;
        player->num_of_draws = 0;
        player->player_status = ACTIVE;
    }
    player->num_of_games++;
    player->num_of_games += victory;
    player->num_of_losses += lose;
    player->num_of_draws += draw;
    if(mapPut(chess->players, player_id, player) == MAP_OUT_OF_MEMORY)
    {
        free(player);
        return CHESS_OUT_OF_MEMORY;
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
    GameId current_game_id = malloc(sizeof(current_game_id));
    if(current_game_id == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    current_game_id->player1_id = first_player;
    current_game_id->player2_id = second_player;
    if (gameParamsValid(chess, tournament_id, first_player, second_player, winner, play_time, current_game_id) != CHESS_SUCCESS)
    {
        return gameParamsValid
    }
    update_players_map(chess, first_player, iswinner);
    update_players_map(chess, first_player, iswinner);

    if(player->num_of_games < max_games)
        return true;
    Game new_game= malloc(sizeof(new_game));
    if (new_game == NULL)
    {
        free(current_game_id);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    new_game->game_id = current_game_id;
    new_game->player1_id = first_player;
    new_game->player2_id = second_player;
    new_game->game_winner = winner;
    new_game->play_time = play_time;
    if (mapPut(current_tournament->games, current_game_id, new_game) == MAP_OUT_OF_MEMORY)
    {
        free(current_game_id);
        free(new_game);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    if (winner == FIRST_PLAYER)
    {
        update_players_map(chess, first_player, 1, 0, 0);
        update_players_map(chess, second_player, 0, 1, 0);
    }
    else if (winner == SECOND_PLAYER)
    {
        update_players_map(chess, second_player, 1, 0, 0);
        update_players_map(chess, first_player, 0, 1, 0);
    }
    else if (winner == DRAW)
    {
        update_players_map(chess, second_player, 0, 0, 1);
        update_players_map(chess, first_player, 0, 0, 1);
    }
    return CHESS_SUCCESS;
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{

}
