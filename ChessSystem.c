#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "map.h"
#include "chessSystem.h"

#define UNDEFINED_WINNER -1;
#define SPACE ' ';

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
    bool active;
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
    int num_of_victories;
    int num_of_losses;
    int num_of_draws;
    PlayerStatus player_status;
    Map games_per_tournament;
} *Player;

enum PlayerStatus_t
{
    ACTIVE,
    REMOVED
};

typedef enum {
    WINNER,
    LOSER,
    TIE,
    REPLACE_DRAW_WITH_TECHNICAL_WIN,
    REPLACE_LOSE_WITH_TECHNICAL_WIN,
    NO_CHANGE_TO_PLAYER_STATE;
} PlayerResult;

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

static bool location_is_valid(const char* tournament_location)
{
    if (strlen(tournament_location) == 0)
    {
        return false;
    }
    if (tournament_location[0] < 'A' || tournament_location[0] > 'Z')
    {
        return false;
    }
    for (int i=1; tournament_location[i] != 0; i++)
    {
        char current_letter = tournament_location[i];
        if (tournament_location[i] != SPACE && (tournament_location[i] < 'a' || tournament_location[i] > 'z'))
        {
            return false;
        }
    }
    return true;
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
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
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
        freeTournament(new_tournament);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    strcpy(tournament_location_copy, tournament_location);
    new_tournament->tournament_location = tournament_location_copy;
    new_tournament->tournament_id = tournament_id;
    new_tournament->max_games_per_player = max_games_per_player;
    new_tournament->winner = UNDEFINED_WINNER;
    new_tournament->games = mapCreate(copyGame, copyGameId, freeGame, freeGameId, compareGameId);
    new_tournament->active = true;
//TODO: copy game + player free game functions
    if (mapPut(chess->tournaments, tournament_id, new_tournament) == MAP_OUT_OF_MEMORY);
    {
        mapDestroy(new_tournament->games);
        freeTournament(new_tournament);
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
    Tournament current_tournament = (Tournament)mapGet(chess->tournaments, tournament_id);
    if (mapContains(current_tournament->games, current_game_id))
    {
        freeGameId(current_game_id);
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (play_time < 0)
    {
        freeGameId(current_game_id);
        return CHESS_INVALID_PLAY_TIME;
    }
    if (!current_tournament->active)
    {
        return CHESS_TOURNAMENT_ENDED
    }
    if (!player_game_count_valid(chess, current_tournamentm ,first_player)
        || !player_game_count_valid(chess, current_tournamentm ,second_player))
    {
        return CHESS_EXCEEDED_GAMES;
    }
    return CHESS_SUCCESS;
}

static bool player_game_count_valid(ChessSystem chess, Tournament tournament,  int player_id)
{
    assert(tournament != NULL && tournament != NULL);
    int max_games = tournament->max_games_per_player;
    assert(chess->players != NULL);

    Player player = (Player)mapGet(chess->players, player_id);
    if(player == NULL)
    {
        return true;
    }
    assert(player->games_per_tournament != NULL);
    int* counted_games = (int*) mapGet(player->games_per_tournament, tournament->tournament_id);
    if( *counted_games < max_games)
    {
        return true;
    }
    return false;
}

static ChessResult update_players_map(ChessSystem chess, int tournament_id, int player_id, PlayerResult_t player_result)
{
    Player player = (Player)mapGet(chess->players, player_id);
    if(player == NULL)
    {
        Player player= malloc(sizeof(new_player));
        if (player == NULL)
        {
            return CHESS_OUT_OF_MEMORY;
        }
        player->player_id = player_id;
        player->num_of_victories = 0;
        player->num_of_losses = 0;
        player->num_of_draws = 0;
        player->player_status = ACTIVE;
        player->games_per_tournament = mapCreate(copyInt, copyInt, freeInt, freeInt);
        if (player->games_per_tournament == NULL)
        {
            free(player);
            return CHESS_OUT_OF_MEMORY;
        }
    }
    if (player_result == WINNER)
    {
        player->num_of_victories ++;
    }
    else if (player_result == LOSER)
    {
        player->num_of_losses ++;
    }
    else if (player_result == DRAW)
    {
        player->num_of_draws ++;
    }
    int* counted_games = (*int) mapGet(player->games_per_tournament, tournament_id);
    if (counted_games == NULL)
    {
        *counted_games = 0;
    }
    mapPut(player->games_per_tournament, tournament_id, &(*counted_games+1);
    if(mapPut(chess->players, player_id, player) == MAP_OUT_OF_MEMORY)
    {
        free(player);
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}


ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner, int play_time)
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
        freeGameId(current_game_id);
        return gameParamsValid;
    }

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
    PlayerResult_t first_player_result = DRAW;
    PlayerResult_t second_player_result = DRAW;
    if (winner == FIRST_PLAYER)
    {
        first_player_result = WINNER;
        second_player_result = LOSER;

    }
    else if (winner == SECOND_PLAYER)
    {
        first_player_result = LOSER;
        second_player_result = WINNER;

    }
    update_players_map(chess, tournament_id, first_player, first_player_result);
    update_players_map(chess, tournament_id, second_player, second_player_result);
    return CHESS_SUCCESS;
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    assert(chess->tournaments != NULL);
    if (tournament_id < 0)
    {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
//    Tournament tournament_to_remove = (Tournament)mapGet(chess->tournaments, tournament_id);
//    MAP_FOREACH(GameId , game_key_iterator, tournament_to_remove->games)
//    {
//        Game game_to_remove = (Game) mapGet(tournament_to_remove->games;
//        assert(game_to_remove != NULL);
//        (game_to_remove, game_key_iterator));
//    }
//    mapDestroy(tournament_to_remove);
    mapRemove(chess->tournaments, tournament_id);
    return CHESS_SUCCESS;
}

ChessResult chessEndTournament (ChessSystem chess, int tournament_id)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id < 0)
    {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    if (mapGet(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament tournament = (Tournament) mapGet(chess->tournaments, tournament_id);
    assert(tournament != NULL)
    Map tournament_players = mapCreate(copyPlayer, copyInt, freePlayer, freeInt, comparePlayer);
    if(tournament_players == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    MAP_FOREACH(GameId, game_key_iterator, tournament->games)
    {
        Game game = (Game) mapGet(tournament->games, game_key_iterator);
        Player player1 = (Player) mapGet(chess->players, game->player1_id);
        Player player2 = (Player) mapGet(chess->players, game->player2_id);
        mapPut(tournament_players, player1, game->player1_id);
        mapPut(tournament_players, player2, game->player2_id);
    }
    int* tournament_winner = (int*) mapGetF(tournament_players, mapGetFirst(tournament_players))
    tournament->winner = copyInt(tournament_winner);
    tournament->active = false;
    mapDestroy(tournament_players);
    return CHESS_SUCCESS;
}

PlayerResult check_player_state_in_game(Winner player_position_in_game, Winner game_winner, bool game_is_new)
{
    if (game_is_new == True)
    {
        if (game_winner == DRAW)
        {
            return TIE;
        }
        if (player_position_in_game == game_winner)
        {
            return WINNER;
        }
        return LOSER;
    }
    else
    {
        if (game_winner == DRAW)
        {
            return REPLACE_DRAW_WITH_TECHNICAL_WIN;
        }
        if (player_position_in_game != game_winner)
        {
            return REPLACE_LOSE_WITH_TECHNICAL_WIN;
        }
    }
    return NO_CHANGE_TO_PLAYER_STATE;

}


ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (player_id < 0)
    {
        return CHESS_INVALID_ID;
    }
    Map players = chess->players;
    assert(players != NULL);
    if (!mapContains(players, player_id))
    {
        return CHESS_PLAYER_NOT_EXIST;
    }
    Map tournaments = chess->tournaments;
    MAP_FOREACH(int*, tournaments_iterator, tournaments)
    {
        Tournament current_tournament = (Tournament) mapGet(tournaments, tournaments_iterator);
        if (current_tournament == NULL && current_tournament->winner != UNDEFINED_WINNER)
        {
            continue;
        }
        else
        {
            MAP_FOREACH(GameId, games_iterator, current_tournament->games)
            {
                Game current_game = (Game) mapGet(games, games_iterator);
                if (current_game == NULL)
                {
                    continue;
                }
                else
                {
                    if (current_game->player1_id != player_id && current_game->player2_id == player_id)
                    {
                        continue;
                    }
                    else
                    {
                        int opponent_id = current_game->player1_id;
                        PlayerResult opponent_new_state = player_state_in_game(FIRST_PLAYER, current_game->game_winner);
                        if (current_game->player2_id == player_id)
                        {
                            opponent_id = current_game->player2_id;
                            opponent_new_state = player_state_in_game(SECOND_PLAYER, current_game->game_winner);
                        }
                        update_players_map(chess, tournaments_iterator, opponent_id, opponent_new_state);
                    }
                }
            }
        }
    }
    assert (players != NULL && mapRemove(players, player_id) == MAP_SUCCESS)
    if (mapRemove(players, player_id) != MAP_SUCCESS)
    {
        return CHESS_SUCCESS;
    }
    return CHESS_SUCCESS;
}

static int comparePlayer(MapKeyElement key_element1, MapKeyElement key_element2)
{
    assert(key_element1 != NULL && key_element2 != NULL && key_element1 != key_element2);
    Player player1 = *(Player) key_element1;
    Player player2 = *(Player) key_element2;
    int player1_score = 2 * player1->num_of_victories + player1->num_of_draws;
    int player2_score = 2 * player2->num_of_victories + player2->num_of_draws;
    if (player1_score == player2_score)
    {
        if (player1->num_of_losses == player2->num_of_losses)
        {
            if (player1->num_of_victories == player2->num_of_victories)
            {
                return (player1->player_id - player2->player_id);
            }
            return (-(player1->num_of_victories - player2->num_of_victories));
        }
        return (player1->num_of_losses - player2->num_of_losses);
    }
    return (-(player1_score - player2_score));
}

static int compareGameId(MapKeyElement key_element1, MapKeyElement key_element2)
{

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

double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    if (chess == NULL)
    {

    }
}

ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{

}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{

}