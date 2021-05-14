#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "./chessSystem.h"
#include "./map.h"
//#include "./tournament.h"
//#include "chessSystem.h"
//#include "map.h"

#define UNDEFINED_WINNER -1
#define SPACE ' '
#define POSITIVE_INT 1
//=============================
struct chess_system_t{
    Map tournaments;
};

typedef enum {
    WINNER,
    LOSER,
    TIE,
    REPLACE_DRAW_WITH_TECHNICAL_WIN,
    REPLACE_LOSE_WITH_TECHNICAL_WIN,
    NO_CHANGE_TO_PLAYER_STATE
} PlayerResult;

typedef struct tournament
{
    char* tournament_location;
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

typedef struct player_level_key
{
    int player_id;
    double level;
} *PlayerLevelKey;

typedef struct game
{
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
//    PlayerStatus player_status;
} *Player;

//=============================================
//        functions declarations
static void* copyInt(void* element);
static void freeInt(void* element);
static int compareInt(void* element1, void* element2);

static MapDataElement copyTournament(MapDataElement data_element);
static void freeTournament(MapDataElement data_element);

static MapDataElement copyGame(MapDataElement data_element);
static void freeGame(MapDataElement data_element);

static MapKeyElement copyGameId(MapKeyElement key_element);
static void freeGameId(MapKeyElement key_element);
static int compareGameId(MapKeyElement key_element1, MapKeyElement key_element2);

static MapKeyElement copyPlayerLevelKey(MapKeyElement key_element);
static void freePlayerLevelKey(MapKeyElement key_element);
static int comparePlayerLevel(MapKeyElement key_element1, MapKeyElement key_element2);

static void* copyPlayer(void* element);
static void freePlayer(void* element);
static int comparePlayer(MapKeyElement key_element1, MapKeyElement key_element2);

static int find_longest_game_time(Tournament tournament);
static double find_average_game_time(Tournament tournament);
static bool tournament_location_is_valid(const char* tournament_location);
static bool player_num_of_games_in_tournament_valid(Tournament tournament,  int player_id);
//=============================================

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

static int compareInt(void* element1, void* element2)
{
    assert(element1 != NULL && element2 != NULL);
    int key1 = *(int*) element1;
    int key2 = *(int*) element2;
    return (key1 - key2);
}

static MapDataElement copyTournament(MapDataElement data_element)
{
    if (data_element == NULL)
    {
        return NULL;
    }
    Tournament new_tournament = malloc(sizeof(*new_tournament));
    if (new_tournament == NULL)
    {
        return NULL;
    }
    *new_tournament = *(Tournament) data_element;
    return new_tournament;
}

static void freeTournament(MapDataElement data_element)
{
    mapDestroy(((Tournament)data_element)->games);
    mapDestroy(((Tournament)data_element)->players);
    free(((Tournament)data_element)->tournament_location);
    free(((Tournament)data_element));
}

static MapDataElement copyGame(MapDataElement data_element)
{
    if (data_element == NULL)
    {
        return NULL;
    }
    Game newGame = malloc(sizeof(*newGame));
    if (newGame == NULL)
    {
        return NULL;
    }
    *newGame = *(Game) data_element;
    return newGame;
}

static void freeGame(MapDataElement data_element)
{
    free((Game) data_element);
}

static MapKeyElement copyGameId(MapKeyElement key_element)
{
    if (key_element == NULL)
    {
        return NULL;
    }
    GameId newGameId = malloc(sizeof(*newGameId));
    if (newGameId == NULL)
    {
        return NULL;
    }
    *newGameId = *(GameId) key_element;
    return newGameId;
}

static void freeGameId(MapKeyElement key_element)
{
    free((GameId) key_element);
}

static int compareGameId(MapKeyElement key_element1, MapKeyElement key_element2)
{
    assert(key_element1 != NULL && key_element2 != NULL && key_element1 != key_element2);
    GameId game_id1 = (GameId) key_element1;
    GameId game_id2 = (GameId) key_element2;
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

static MapKeyElement copyPlayerLevelKey(MapKeyElement key_element)
{
    if (key_element == NULL)
    {
        return NULL;
    }
    PlayerLevelKey newPlayerLevelKey = malloc(sizeof(*newPlayerLevelKey));
    if (newPlayerLevelKey == NULL)
    {
        return NULL;
    }
    *newPlayerLevelKey = *(PlayerLevelKey) key_element;
    return newPlayerLevelKey;
}

static void freePlayerLevelKey(MapKeyElement key_element)
{
    free((PlayerLevelKey) key_element);
}

static int comparePlayerLevel(MapKeyElement key_element1, MapKeyElement key_element2)
{
    assert(key_element1 != NULL && key_element2 != NULL && key_element1 != key_element2);
    PlayerLevelKey player1 = (PlayerLevelKey) key_element1;
    PlayerLevelKey player2 = (PlayerLevelKey) key_element2;
    if(player1->level == player2->level)
    {
        return (player1->player_id - player2->player_id);
    }
    return (int)(-(player1->level - player2->level));
}

static void* copyPlayer(void* element)
{
    if (element == NULL)
    {
        return NULL;
    }
    Player newPlayer = malloc(sizeof(*newPlayer));
    if (newPlayer == NULL)
    {
        return NULL;
    }
    *newPlayer = *(Player) element;
    return newPlayer;
}

static void freePlayer(void* element)
{
    free((Player) element);
}

static int comparePlayer(MapKeyElement key_element1, MapKeyElement key_element2)
{
    assert(key_element1 != NULL && key_element2 != NULL && key_element1 != key_element2);
    Player player1 = (Player) key_element1;
    Player player2 = (Player) key_element2;
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

static int find_longest_game_time(Tournament tournament)
{
    assert(tournament != NULL);
    int longest_game_time = 0;
    MAP_FOREACH(GameId, game_id, tournament->games)
    {
        Game game = mapGet(tournament->games, game_id);
        assert(game != NULL);
        if (game->play_time > longest_game_time)
        {
            longest_game_time = game->play_time;
        }
    }
    return longest_game_time;
}

static double find_average_game_time(Tournament tournament)
{
    assert(tournament != NULL);
    double average_game_time = 0;
    MAP_FOREACH(GameId, game_id, tournament->games)
    {
        Game game = mapGet(tournament->games, game_id);
        assert(game != NULL);
        average_game_time += game->play_time;
    }
    return (double)(average_game_time / mapGetSize(tournament->games));
}

static double calculate_player_level(Player player)
{
    assert(player != NULL);
    return (double)((6 * player->num_of_victories) - (10 * player->num_of_losses) + (2 * player->num_of_draws)) / (player->num_of_games);
}

static bool player_already_exists(ChessSystem chess, int player_id)
{
    assert(chess != NULL && player_id > 0);
    MAP_FOREACH(int*, tournament_key_iterator, chess->tournaments)
    {
        Tournament tournament = (Tournament) mapGet(chess->tournaments, tournament_key_iterator);
        assert(tournament != NULL);
        if (mapContains(tournament->players, &player_id))
        {
            return true;
        }
    }
    return false;
}

ChessSystem chessCreate()
{
    ChessSystem chess_system = malloc(sizeof(ChessSystem));
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

void chessDestroy(ChessSystem chess)
{
    if (chess == NULL)
    {
        return;
    }
    mapDestroy(chess->tournaments);
    free(chess);
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
    if (!tournament_location_is_valid(tournament_location))
    {
        return CHESS_INVALID_LOCATION;
    }
    if (mapContains(chess->tournaments, &tournament_id))
    {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    return CHESS_SUCCESS;
}

static bool tournament_location_is_valid(const char* tournament_location)
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
    ChessResult tournament_params_result = tournamentParamsValid(chess, tournament_id, tournament_location);
    if (tournament_params_result != CHESS_SUCCESS)
    {
        return tournament_params_result;
    }
    Tournament tournament= malloc(sizeof(*tournament));
    if (tournament == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    char* tournament_location_copy = malloc(strlen(tournament_location) + 1);
    if (tournament_location_copy == NULL)
    {
        freeTournament(tournament);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    strcpy(tournament_location_copy, tournament_location);
    tournament->tournament_location = tournament_location_copy;
    tournament->max_games_per_player = max_games_per_player;
    tournament->winner = UNDEFINED_WINNER;
    tournament->active = true;
    tournament->games = mapCreate(copyGame, copyGameId, freeGame, freeGameId, compareGameId);
    tournament->players = mapCreate(copyPlayer, copyInt, freePlayer, freeInt, compareInt);
    MapResult result = mapPut(chess->tournaments, &tournament_id, tournament);
    freeTournament(tournament);
    if (result == MAP_OUT_OF_MEMORY)
    {
        mapDestroy(tournament->games);
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}

static ChessResult gameParamsValid(ChessSystem chess, int tournament_id, int first_player,
                                   int second_player, Winner winner, int play_time, GameId game_id)
{
    if (tournament_id < 0 || first_player < 0 || second_player < 0 || first_player == second_player)
    {
        return CHESS_INVALID_ID;
    }
    if (winner != FIRST_PLAYER && winner != SECOND_PLAYER && winner != DRAW)
    {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, &tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    assert(chess->tournaments != NULL && tournament_id > 0 && !mapContains(chess->tournaments, &tournament_id));
    Tournament tournament = (Tournament) mapGet(chess->tournaments, &tournament_id);
    assert(tournament != NULL);
    if (!(tournament->active))
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    if (mapContains(tournament->games, game_id))
    {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (play_time < 0)
    {
        return CHESS_INVALID_PLAY_TIME;
    }
    if (!player_num_of_games_in_tournament_valid(tournament ,first_player)
        || !player_num_of_games_in_tournament_valid(tournament ,second_player))
    {
        return CHESS_EXCEEDED_GAMES;
    }
    return CHESS_SUCCESS;
}

static bool player_num_of_games_in_tournament_valid(Tournament tournament,  int player_id)
{
    assert(tournament != NULL && player_id > 0);
    Player player = (Player)mapGet(tournament->players, &player_id);
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
    Player player = (Player)mapGet(players, &player_id);
    if(player == NULL)
    {
        player = malloc(sizeof(*player));
        if (player == NULL)
        {
            return CHESS_OUT_OF_MEMORY;
        }
        player->player_id = player_id;
        player->num_of_games = 0;
        player->num_of_victories = 0;
        player->num_of_losses = 0;
        player->num_of_draws = 0;
//        player->player_status = ACTIVE;
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
    else if (player_result == TIE)
    {
        player->num_of_draws ++;
    }
    else if (player_result == REPLACE_DRAW_WITH_TECHNICAL_WIN)
    {
        player->num_of_draws --;
        player->num_of_victories ++;
    }
    else if (player_result == REPLACE_LOSE_WITH_TECHNICAL_WIN)
    {
        player->num_of_losses --;
        player->num_of_victories ++;
    }
    MapResult result = mapPut(players, &player_id, player);
    free(player);
    if(result == MAP_OUT_OF_MEMORY)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}

static PlayerResult check_player_result_in_game(Winner player_position_in_game, Winner game_winner, bool game_is_new)
{
    if (game_is_new == true)
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

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player_id, int second_player_id, Winner winner, int play_time)
{
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    GameId game_id = malloc(sizeof(*game_id));
    if(game_id == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    game_id->player1_id = first_player_id;
    game_id->player2_id = second_player_id;
    ChessResult game_params_result = gameParamsValid(chess, tournament_id, first_player_id, second_player_id, winner, play_time, game_id);
    if (game_params_result != CHESS_SUCCESS)
    {
        freeGameId(game_id);
        return game_params_result;
    }
    Game new_game= malloc(sizeof(*new_game));
    if (new_game == NULL)
    {
        free(game_id);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    new_game->player1_id = first_player_id;
    new_game->player2_id = second_player_id;
    new_game->game_winner = winner;
    new_game->play_time = play_time;
    Tournament current_tournament = mapGet(chess->tournaments, &tournament_id);
    assert(current_tournament != NULL);
    MapResult result = mapPut(current_tournament->games, game_id, new_game);
    free(game_id);
    free(new_game);
    if (result == MAP_OUT_OF_MEMORY)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    PlayerResult first_player_result = check_player_result_in_game(FIRST_PLAYER, winner, true);
    PlayerResult second_player_result = check_player_result_in_game(SECOND_PLAYER, winner, true);
    ChessResult update_result1 = update_players_map(current_tournament->players, first_player_id, first_player_result);
    ChessResult update_result2 = update_players_map(current_tournament->players, second_player_id, second_player_result);
    if (update_result1 == CHESS_OUT_OF_MEMORY || update_result2 == CHESS_OUT_OF_MEMORY)
    {
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
    if (!mapContains(chess->tournaments, &tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    mapRemove(chess->tournaments, &tournament_id);
    return CHESS_SUCCESS;
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
    if (!(player_already_exists(chess, player_id)))
    {
        return CHESS_PLAYER_NOT_EXIST;
    }
    MAP_FOREACH(int*, tournaments_id, chess->tournaments)
    {
        Tournament tournament = (Tournament) mapGet(chess->tournaments, tournaments_id);
        assert(tournament != NULL);
        if (tournament->active && mapContains(tournament->players, &player_id))
        {
            // we get here only if the tournament didnt end and the requested player to remove exists in the tournament
            MAP_FOREACH(GameId, games_id, tournament->games)
            {
                Game game = (Game) mapGet(tournament->games, games_id);
                assert(game != NULL);
                if (game->player1_id == player_id || game->player2_id == player_id)
                {
                    int opponent_id = game->player1_id;
                    PlayerResult opponent_new_state = check_player_result_in_game(FIRST_PLAYER, game->game_winner, false);
                    if (game->player1_id == player_id)
                    {
                        opponent_id = game->player2_id;
                        opponent_new_state = check_player_result_in_game(SECOND_PLAYER, game->game_winner, false);
                    }
                    update_players_map(tournament->players, opponent_id, opponent_new_state);
                    MapResult result = mapRemove(tournament->players, &player_id);
//                    assert(result == MAP_SUCCESS);
                    if (result != MAP_SUCCESS)
                    {
                        return CHESS_NULL_ARGUMENT;
                    }
                }
            }
        }
    }
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
    if (!mapContains(chess->tournaments, &tournament_id))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament tournament = (Tournament) mapGet(chess->tournaments, &tournament_id);
    assert(tournament != NULL);
    Map ranked_tournament_players = mapCreate(copyPlayer, copyInt, freePlayer, freeInt, comparePlayer);
    if(ranked_tournament_players == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    MAP_FOREACH(int* , player_id, tournament->players)
    {
        Player player = (Player) mapGet(tournament->players, player_id);
        assert(player != NULL);
        if (mapPut(ranked_tournament_players, player, player_id) == MAP_OUT_OF_MEMORY)
        {
            mapDestroy(ranked_tournament_players);
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
    }
    tournament->winner = *((int*)mapGet(ranked_tournament_players, mapGetFirst(ranked_tournament_players)));
    tournament->active = false;
    mapDestroy(ranked_tournament_players);
    return CHESS_SUCCESS;
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
        *chess_result = CHESS_PLAYER_NOT_EXIST;
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
    return ((double)player_game_time_count / player_num_of_games);
}

static Map create_all_players_map(ChessSystem chess)
{
    Map all_players = mapCreate(copyPlayer, copyInt, freePlayer, freeInt, compareInt);
    if(all_players == NULL)
    {
        return NULL;
    }
    MAP_FOREACH(int*, tournament_id, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, tournament_id);
        assert(tournament != NULL);
        MAP_FOREACH(int*, player_id, tournament->players)
        {
            if(update_players_map(all_players, *player_id, NO_CHANGE_TO_PLAYER_STATE) == CHESS_OUT_OF_MEMORY)
            {
                mapDestroy(all_players);
                return NULL;
            }
        }
    }
    return all_players;
}

ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
    Map all_players = create_all_players_map(chess);
    if(all_players == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    Map ranked_players = mapCreate(copyPlayerLevelKey, copyPlayerLevelKey, freePlayerLevelKey, freePlayerLevelKey, comparePlayerLevel);
    if(ranked_players == NULL)
    {
        mapDestroy(all_players);
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    MAP_FOREACH(int*, player_id, all_players)
    {
        double player_level = calculate_player_level(mapGet(all_players, player_id));
        PlayerLevelKey player_level_key = malloc(sizeof(*player_level_key));
        if (player_level_key == NULL)
        {
            mapDestroy(all_players);
            mapDestroy(ranked_players);
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
        player_level_key->player_id = *player_id;
        player_level_key->level = player_level;
        MapResult result = mapPut(ranked_players, player_level_key, player_level_key);
        free (player_level_key);
        mapDestroy(all_players);
        if(result == MAP_OUT_OF_MEMORY)
        {
            mapDestroy(ranked_players);
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
    }
    MAP_FOREACH(PlayerLevelKey, player_level_key, ranked_players)
    {
        fprintf(file, "%d %.2f\n", player_level_key->player_id, player_level_key->level);
    }
    mapDestroy(ranked_players);
    return CHESS_SUCCESS;
}

static bool existing_finished_tournament(ChessSystem chess)
{
    MAP_FOREACH(int*, tournament_id, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, tournament_id);
        if(!(tournament->active))
        {
            return true;
        }
    }
    return false;
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file) {
    if (chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (!(existing_finished_tournament(chess)))
    {
        return CHESS_NO_TOURNAMENTS_ENDED;
    }
    FILE *file = fopen(path_file, "w");
    if (file == NULL)
    {
        return CHESS_SAVE_FAILURE;
    }
    MAP_FOREACH (int*, tournament_id, chess->tournaments)
    {
        Tournament tournament = mapGet(chess->tournaments, tournament_id);
        assert(tournament != NULL);
        if(!(tournament->active))
        {
            fprintf(file, "%d\n%d\n%.2f\n%s\n%d\n%d\n",
                    tournament->winner,
                    find_longest_game_time(tournament),
                    find_average_game_time(tournament),
                    tournament->tournament_location,
                    mapGetSize(tournament->games),
                    mapGetSize(tournament->players));
        }
    }
    return CHESS_SUCCESS;
}