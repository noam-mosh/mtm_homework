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
};

typedef struct tournament
{
    int tournament_id;
    const char* tournament_location;
    int max_games_per_player;
    int winner;
    bool active;
    Map games;
    Map players;
} *Tournament;

typedef struct game_id
{
    int player1_id;
    int player2_id;
} *GameId;

//TODO: player_level_key?
typedef struct player_level_id
{
    int player_id;
    double level;
} *PlayerLevelId;

typedef struct game
{
    game_id game_id;
    int player1_id;
    int player2_id;
    int play_time;
    Winner game_winner;
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

typedef enum
{
    ACTIVE,
    REMOVED
}PlayerStatus;

typedef enum
{
    WINNER,
    LOSER,
    TIE
}PlayerResult;

=============================================

static void* copyInt(void* element)
{
    if (element == NULL) {
        return NULL;
    }
    int *newInt = malloc(sizeof(int));
    if (newInt == NULL)
    {
        return NULL;
    }
    *newInt = *(int *) element;
    return newInt;
}

static void freeInt(void* element)
{
    free(element);
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
    mapDestroy(data_element->players);
    free(data_element->tournament_location);
    free(data_element);
}

static int compareInt(void* element1, void* element2)
{
    assert(element1 != NULL && element2 != NULL);
    int key1 = *(* int) element1;
    int key2 = *(* int) element2;
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
    newGameId->player1_id = ((GameId) key_element)->player1_id;
    newGameId->player2_id = ((GameId) key_element)->player2_id;
    return newGameId;
}

static MapKeyElement copyPlayerLevelId(MapKeyElement key_element)
{
    if (key_element == NULL)
    {
        return NULL;
    }
    PlayerLevelId newPlayerLevelId = malloc(sizeof(newPlayerLevelId));
    if (newPlayerLevelId == NULL)
    {
        return NULL;
    }
    newPlayerLevelId->player_id = ((PlayerLevelId) key_element)->player_id;
    newPlayerLevelId->level = ((PlayerLevelId) key_element)->level;
    return newPlayerLevelId;
}

static void* copyPlayer(void* element)
{
    if (element == NULL)
    {
        return NULL;
    }
    Player newPlayer = malloc(sizeof(newPlayer));
    if (newPlayer == NULL)
    {
        return NULL;
    }
    newPlayer->player_id = ((Player) element)->player_id;
    newPlayer->num_of_games = ((Player) element)->num_of_games;
    newPlayer->num_of_victories = ((Player) element)->num_of_victories;
    newPlayer->num_of_losses = ((Player) element)->num_of_losses;
    newPlayer->num_of_draws = ((Player) element)->num_of_draws;
    newPlayer->player_status = ((Player) element)->player_status;
    return newPlayer;
}

static void freePlayer(void* element)
{
    free(element);
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

bool player_already_exists(ChessSystem chess, int player_id)
{
    assert(chess != NULL && player_id > 0);
    MAP_FOREACH(int*, tournament_key_iterator, chess->tournament)
    {
        Tournament tournament = (Tournament) mapGet(chess->tournament, tournament_key_iterator);
        assert(tournament != NULL);
        if (mapContains(tournament->players, player_id))
        {
            return true;
        }
    }
    return false;
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
    chess_system->tournaments = tournaments;
    return chess_system;
}

static ChessResult tournamentParamsValid(ChessSystem chess, int tournament_id, const char* tournament_location)
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
    return CHESS_SUCCESS;
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    ChessResult game_params_validation = tournamentParamsValid(chess, tournament_id, tournament_location);
    if (game_params_validation != CHESS_SUCCESS)
    {
        return game_params_validation;
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
    new_tournament->active = true;
    new_tournament->games = mapCreate(copyGame, copyGameId, freeGame, freeGameId, compareGameId);
    new_tournament->players = mapCreate(copyPlayer, copyInt, freePlayer, freeInt, compareInt);
    if (mapPut(chess->tournaments, tournament_id, new_tournament) == MAP_OUT_OF_MEMORY)
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
    if (tournament_id < 0 || first_player < 0 || second_player < 0 || first_player = second_player)
    {
        return CHESS_INVALID_ID;
    }
    if (winner != FIRST_PLAYER && winner != SECOND_PLAYER && winner != DRAW)
    {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    assert(chess->tournaments != NULL && tournament_id > 0 && !mapContains(chess->tournaments, tournament_id));
    Tournament current_tournament = (Tournament) mapGet(chess->tournaments, tournament_id);
    assert(current_tournament != NULL);
    if (!current_tournament->active)
    {
        return CHESS_TOURNAMENT_ENDED
    }
    if (mapContains(current_tournament->games, game_id))
    {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (play_time < 0)
    {
        return CHESS_INVALID_PLAY_TIME;
    }

    if (!player_num_of_games_in_tournament_valid(current_tournamentm ,first_player)
        || !player_num_of_games_in_tournament_valid(current_tournamentm ,second_player))
    {
        return CHESS_EXCEEDED_GAMES;
    }
    return CHESS_SUCCESS;
}

static bool player_num_of_games_in_tournament_valid(Tournament tournament,  int player_id)
{
    assert(tournament != NULL && player_id > 0);
    Player player = (Player)mapGet(tournament->players, player_id);
    if(player == NULL)
    {
        // it means player is new in this tournament;
        return true;
    }
    if(player->num_of_games < tournament->max_games_per_player)
    {
        return true;
    }
    return false;
}

static ChessResult update_players_map(Map players, int player_id, PlayerResult player_result)
{
    Player player = (Player)mapGet(players, player_id);
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
    if(mapPut(players, player_id, player) == MAP_OUT_OF_MEMORY)
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
    GameId game_id = malloc(sizeof(game_id));
    if(game_id == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    game_id->player1_id = first_player;
    game_id->player2_id = second_player;
    ChessResult game_params_validation = gameParamsValid(chess, tournament_id, first_player, second_player, winner, play_time, game_id);
    if (game_params_validation != CHESS_SUCCESS)
    {
        freeGameId(game_id);
        return game_params_validation;
    }
    Game new_game= malloc(sizeof(new_game));
    if (new_game == NULL)
    {
        free(game_id);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    new_game->game_id = game_id;
    new_game->player1_id = first_player;
    new_game->player2_id = second_player;
    new_game->game_winner = winner;
    new_game->play_time = play_time;
    Tournament current_tournament = mapGet(chess->tournaments, tournament_id);
    if (mapPut(current_tournament->games, game_id, new_game) == MAP_OUT_OF_MEMORY)
    {
        free(game_id);
        free(new_game);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    PlayerResult first_player_result = check_player_state_in_game(FIRST_PLAYER, winner, true);
    PlayerResult second_player_result = check_player_state_in_game(SECOND_PLAYER, winner, true);
    ChessResult update_result1 = update_players_map(current_tournament->players, first_player, first_player_result);
    ChessResult update_result2 = update_players_map(current_tournament->players, second_player, second_player_result);
    if (update_result1 == CHESS_OUT_OF_MEMORY || update_result2 == CHESS_OUT_OF_MEMORY)
    {
        free(game_id);
        free(new_game);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
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
    Tournament tournament = (Tournament) mapGet(chess->tournaments, tournament_id);
    assert(tournament != NULL)
    Map ranked_tournament_players = mapCreate(copyPlayer, copyInt, freePlayer, freeInt, comparePlayer);
    if(ranked_tournament_players == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    MAP_FOREACH(int* , player_iterator, tournament->players)
    {
        Player player = (Player) mapGet(tournament->players, player_iterator);
        assert(player != NULL);
        if (mapPut(ranked_tournament_players, player, player_iterator) == MAP_OUT_OF_MEMORY)
        {
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
    }
    tournament->winner = (int*) mapGet(ranked_tournament_players, mapGetFirst(ranked_tournament_players))
    tournament->active = false;
    mapDestroy(ranked_tournament_players);
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
    //TODO: figure out what to return if we get errors- zero?
    if (chess == NULL)
    {
        *chess_result = CHESS_NULL_ARGUMENT;
        return 0;
    }
    if (player_id < 0)
    {
        *chess_result = CHESS_INVALID_ID;
        return 0;
    }
    if(!player_already_exists(chess, player_id))
    {
        *chess_result = CHESS_PLAYER_NOT_EXIST
        return 0;
    }
    int player_num_of_games = 0;
    int player_game_time_count = 0;
    MAP_FOREACH(int*, tournament_id, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, tournament_id);
        assert(tournament != NULL);
        MAP_FOREACH(int*, game_id, tournament->games)
        {
            Game game = mapGet(tournament->games, game_id);
            assert(game != NULL);
            if (player_id == game->player1_id || player_id == game->player2_id)
            {
                player_num_of_games++;
                player_game_time_count += game->play_time;
            }
        }
    }
    *chess_result = CHESS_SUCCESS;
    return (player_game_time_count / player_num_of_games);
}

Map create_all_players_map(ChessSystem chess)
{
    Map all_players = mapCreate(copyPlayer, copyInt, freePlayer, freeInt, compareInt);
    if(all_players == MAP_OUT_OF_MEMORY)
    {
        return NULL;
    }
    MAP_FOREACH(int*, tournament_id, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, tournament_id);
        assert(tournament != NULL);
        MAP_FOREACH(int*, game_id, tournament->games)
        {
            Game game = mapGet(tournament->games, game_id);
            assert(game != NULL);
            if(update_players_map(all_players, player_id, game->game_winner) == CHESS_OUT_OF_MEMORY)
            {
                mapDestroy(all_players)
                return NULL;
            }
        }
    }
    return all_players;
}

ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
    Map all_players = create_all_players_map(ChessSystem chess);
    if(all_players == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    Map ranked_players = mapCreate(copyPlayerLevel, copyPlayerLevel, freePlayerLevel, freePlayerLevel, comparePlayerLevel);
    MAP_FOREACH(int*, player_id, all_players)
    {
        Player player = mapGet(all_players, player_id);
        assert(player != NULL);
        int player_level = ((6 * player->num_of_victories) - (10 * player->num_of_losses) + (2 * player->num_of_draws)) / (player->num_of_games);
        PlayerLevelId player_level_id = malloc(sizeof(player_level_id));
        if (player_level_id == NULL)
        {
            mapDestroy(all_players);
            mapDestroy(ranked_players);
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
        player_level_id->player_id = player_id;
        player_level_id->level = player_level;
        if(mapPut(ranked_players, player_level_id, player_level_id) == CHESS_OUT_OF_MEMORY)
        {
            mapDestroy(all_players);
            mapDestroy(ranked_players);
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
    }
    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        return NULL;
    }
    return CHESS_SUCCESS;
}

int comparePlayerLevel(MapKeyElement key_element1, MapKeyElement key_element2)
{
    assert(key_element1 != NULL && key_element2 != NULL && key_element1 != key_element2);
    PlayerLevelId player1 = *(PlayerLevelId) key_element1;
    PlayerLevelId player2 = *(PlayerLevelId) key_element2;
    if(player1->level == player2->level)
    {
        return (player1->player_id - player2->player_id);
    }
    return (-(player1->level - player2->level));
}



void* copyPlayerLevel(void* element)
{

}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{

}

HistoricalDate readEvents(char* filename) {
    FILE* fd = fopen(filename, "r");
    if (!fd) {
        return NULL;
    }     
    HistoricalDate history = NULL;
    char buffer[BUFF_SIZE] = "";
    Date inputDate;
    while (dateRead(&inputDate,fd) && fgets(buffer,BUFF_SIZE,fd) != NULL) {
        Event newEvent = eventCreate(buffer);
        history = historicalDateAddEvent(history, inputDate, newEvent);
    }
    fclose(fd);
    return history;
