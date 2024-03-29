/*!
 * \file main.cpp
 * \brief SandBox mode for Hypersonic
 * \author Yuriu5
 * \version 1.0
 */


#define WIDTH   13
#define HEIGHT  11
#define LOOP_MAX 200

#define ENTITY_PLAYER  0
#define ENTITY_BOMB    1
#define ENTITY_ITEMS   2

#define TYPE_ITEM_RANGE 1
#define TYPE_ITEM_BOMB  2

#define CELL_FLOOR '.'
#define BOX        '0'
#define BOX_ITEM_RANGE '1'
#define BOX_ITEM_BOMB  '2'
#define CELL_WALL  'X'
#define PLAYER_0   'a'
#define PLAYER_1   'b'
#define PLAYER_2   'c'
#define PLAYER_3   'd'
#define BOMB       'B'
#define EXPLOSED   'E'
#define ITEM_RANGE 'z'
#define ITEM_BOMB  'w'

typedef struct _tBomb{
    int x;
    int y;
    int range;
    int owner;
    int tick;
}tBomb;

typedef struct _tObject{
    int x;
    int y;
    int type;
}tObject;

typedef struct _tPlayer{
    int x;
    int y;
    int range;
    int nbBombs;
    int score;
    int id;
}tPlayer;

int main(int argc, char *argv[]){
    
    int     nLoop = 0;
    bool    deathPlayer = false;
    
    char    map[HEIGHT][WIDTH];
    map[0] = {'.', '.', '.', '.', '1', '.', '.', '.', '1', '.', '.', '.', '.'};
    map[1] = {'.', 'X', '.', 'X', '0', 'X', '.', 'X', '0', 'X', '.', 'X', '.'};
    map[2] = {'.', '.', '.', '1', '0', '.', '.', '.', '0', '1', '.', '.', '.'};
    map[3] = {'2', 'X', '.', 'X', '2', 'X', '2', 'X', '2', 'X', '.', 'X', '2'};
    map[4] = {'1', '.', '.', '2', '.', '.', '.', '.', '.', '2', '.', '.', '1'};
    map[5] = {'.', 'X', '0', 'X', '.', 'X', '.', 'X', '.', 'X', '0', 'X', '.'};
    map[6] = map[4];
    map[7] = map[3];
    map[8] = map[2];
    map[9] = map[1];
    map[10] = map[0];
    
    vector<tBomb>    listBombs;
    vector<tObject>  listObjects;
    vector<tPlayer>  listPlayer;
    
    listPlayer.push_back(tPlayer(0,0,3,1,0));
    
    while((nLoop < LOOP_MAX) && (false == deathPlayer)){
        
        //Update tick
        updateTick(&listBombs);
        //Explose each bomb at 0 // Update score
        exploseBomb(&listBombs, map);
        //Check if player dead
        deathPlayer = checkDeath(map,listPlayers);
        //Update map with explosion
        //Display map to console
        //Give input to each player
        //Do action that each player output (add new bomb / placement player)
        
    
        nLoop++;
    }
}


/*! \fn void updateTick(vector<tBomb> &listBombs)
 *  \brief Update tick of each bomb
 *  \param[in,out] listBombs Reference to the vector containing the bomb to update
 *  \return Nothing
 */
void updateTick(vector<tBomb> &listBombs){
    for(int i=0; i < listBombs.size(); i++){
        listBombs[i].tick--;
    }
}

/*! \brief Explose the bomb\n
 *         Check the tick of each bomb and explose them if tick is equal to zero.\n 
 *         Explose bomb that are caught in the range of a bomb exploding.\n 
 *         Update the score of each player.\n
 *         Remove the bombs explosed from the vector.\n
 *  \param[in,out] listbombs    Reference to the vector containing the bomb ingame
 *  \param[in,out] listPlayers  Reference to the vector of player
 *  \param[in,out] map          Point to the map matrix
 *  \param[in]     updateMap    Bool to force the update of the map during explosion
 *  \return  Nothing
 */
void exploseBomb(vector<tBomb> &listBombs, vector<tPlayer> &listPlayers, char **map, bool updateMap = true){
    queue<int> fifoBombToExplose;
    int idxBombToRemove[100];
    int nbBombToRemove = 0;
    for(int i=0; i < listBombs.size(); i++){
        if(listBombs[i].tick == 0){
            fifoBombToExplose.push(i);
            idxBombToRemove[nbBombToRemove] = i;
            nbBombToRemove++;
        }
    }
    while(false == fifoBombToExplose.empty()){
        int idxBomb = fifoBombToExplose.pop();
        char nbExplosed = 0;
        bool detectStop = false;
        char i = listBombs[idxBomb].x;
        while((false == detectStop) && (i < WIDTH) && (i < (listBombs[idxBomb].x+listBombs[idxBomb].range))){
            i++;
            if(BOMB == map[listBombs[idxBomb].y][i]){
                int idx = findIndexBomb(listBombs, listBombs[idxBomb].y, i);
                fifoBombToExplose.push(idx);
                idxBombToRemove[nbBombToRemove] = idx;
                nbBombToRemove++;                
            }
            if(true == isObstableForBomb(map[listBombs[idxBomb].y][i])){
                detectStop = true;
                if(true == isABox(map[listBombs[idxBomb].y][i]))
                    nbExplosed++;
            }
            if((true == canBeExplosed(map[listBombs[idxBomb].y][i])) && (true == updateMap))
                map[listBombs[idxBomb].y][i] = EXPLOSED;
        }
        detectStop = false;
        i = listBombs[idxBomb].x;
        while((false == detectStop) && (i >= 0) && (i > (listBombs[idxBomb].x-listBombs[idxBomb].range))){
            i--;
            if(BOMB == map[listBombs[idxBomb].y][i]){
                int idx = findIndexBomb(listBombs, listBombs[idxBomb].y, i);
                fifoBombToExplose.push(idx);
                idxBombToRemove[nbBombToRemove] = idx;
                nbBombToRemove++;                                
            }            
            if(true == isObstableForBomb(map[listBombs[idxBomb].y][i])){
                detectStop = true;
                if(true == isABox(map[listBombs[idxBomb].y][i]))
                    nbExplosed++;
            }
            if((true == canBeExplosed(map[listBombs[idxBomb].y][i])) && (true == updateMap))
                map[listBombs[idxBomb].y][i] = EXPLOSED;
        }
        detectStop = false;
        char j = listBombs[idxBomb].y;
        while((false == detectStop) && (j < HEIGHT) && (j < (listBombs[idxBomb].y+listBombs[idxBomb].range))){
            j++;         
            if(BOMB == map[j][listBombs[idxBomb].x]){
                int idx = findIndexBomb(listBombs, j, listBombs[idxBomb].x);
                fifoBombToExplose.push(idx);
                idxBombToRemove[nbBombToRemove] = idx;
                nbBombToRemove++;                                
            }            
            if(true == isObstableForBomb(map[j][listBombs[idxBomb].x])){
                detectStop = true;
                if(true == isABox(map[j][listBombs[idxBomb].x]))
                    nbExplosed++;
            }
            if((true == canBeExplosed(map[j][listBombs[idxBomb].x])) && (true == updateMap))
                map[j][listBombs[idxBomb].x] = EXPLOSED;

        }
        detectStop = false;
        j = listBombs[idxBomb].y;
        while((false == detectStop) && (j >= 0) && (j > (listBombs[idxBomb].y-listBombs[idxBomb].range))){
            j--;            
            if(BOMB == map[j][listBombs[idxBomb].x]){
                int idx = findIndexBomb(listBombs, j, listBombs[idxBomb].x);
                fifoBombToExplose.push(idx);
                idxBombToRemove[nbBombToRemove] = idx;
                nbBombToRemove++;                                
            }               
            if(true == isObstableForBomb(map[j][listBombs[idxBomb].x])){
                detectStop = true;
                if(true == isABox(map[j][listBombs[idxBomb].x]))
                    nbExplosed++;
            }
            if((true == map.canBeExplosed(j,listBombs[idxBomb].x)) && (true == updateMap)) 
                map[j][listBombs[idxBomb].x] = EXPLOSED;

        } 
        if(true == updateMap)
            map[listBombs[idxBomb].y][listBombs[idxBomb].x] = EXPLOSED;
            
        //Update score of the corresponding player    
        for(int i=0; i < listPlayers.size(); i++){
            if(listPlayers[i].id == listBombs[idxBomb].owner) listPlayers[i].score += nbExplosed;
        }
    }            
    
    //Sort idxBombToRemove (sort max to min)
    int idxBombToRemoveSorted[100];
    for(int j=0; j < nbBombToRemove; j++){
        int max = -1;
        int idxMax = -1;        
        for(int i=0; i < nbBombToRemove; i++){
            if(idxBombToRemove[i] > max){
                max = idxBombToRemove[i];
                idxMax = i;
            }
        };
        idxBombToRemoveSorted[j] = max;
        idxBombToRemove[idxMax] = -2;
    }
    
    //Remove bomb explosed from the vector
    for(int i=0; i < nbBombToRemove; i++){
        idxToRemove = idxBombToRemoveSorted[i];
        listBombs.erase(listBombs.begin()+idxToRemove);
    }
}

bool checkDeath(char **map, vector<tPlayer> listPlayers){
    bool retDeath = false;
    for(int i=0; i < listPlayers.size(); i++){
        if(EXPLOSED == map[listPlayers[i].y][listPlayers[i].x])
            retDeath = true;
    }
    return retDeath;
}

void printMap(char **map, vector<tPlayer> &listPlayer)
    char copyMap[HEIGHT][WIDTH];
    memcpy(copyMap, map, HEIGHT*WIDTH*sizeof(char));
    for(int i=0; i < listPlayer.size(); i++){
        switch(listPlayer[i]){
            case 0: copyMap[listPlayer[i].y][listPlayer[i].x] = PLAYER_0;break;
            case 1: copyMap[listPlayer[i].y][listPlayer[i].x] = PLAYER_1;break;
            case 2: copyMap[listPlayer[i].y][listPlayer[i].x] = PLAYER_2;break;
            case 3: copyMap[listPlayer[i].y][listPlayer[i].x] = PLAYER_3;break;
        }
    }
          
    for(int i = 0;i < HEIGHT; i++){
        char row[WIDTH+1];
        for(int j = 0; j < WIDTH; j++){
            row[j] = copyMap[i][j];
        }
        row[WIDTH] = '\0';
        std::cout << row << std::endl;
    }
}

int findIndexBomb(vector<Bomb> &listBombs,y,x){
    int retIdx = -1;
    for(int i=0; i < listBombs.size(); i++){
        if(listBombs.x == x && listBombs.y == y)
            retIdx = i;
    }
    return retIdx;
}

inline bool isABox(char c){
    bool ret = false;
    if( (BOX == c) || 
        (BOX_ITEM_RANGE == c) ||
        (BOX_ITEM_BOMB == c)
    )
        ret = true;
    return ret;
}

inline bool isObstableForBomb(char c){
    bool ret = false;
    if( (BOX == c)  ||
        (BOMB == c) ||
        (BOX_ITEM_RANGE == c) ||
        (BOX_ITEM_BOMB == c) ||
        (ITEM_RANGE == c) ||
        (ITEM_BOMB == c) ||
        (CELL_WALL == c)
    )
        ret = true;
    return ret;
}

inline bool canBeExplosed(char c){
    bool ret = true;
    if( CELL_WALL == c)
        ret = false;
    return ret;
}

    
