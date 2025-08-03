#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unordered_set>

using namespace std;

//SETTING UP DATAFRAMES
vector<bool> prisoners_in_jail(3, false);
vector<string> keys_collected;
bool rotation_control_unlocked = false;

struct tile{
    char content;
};
struct board{
    tile tiles[3][3];

};
typedef enum {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
} face;
struct cube{
    board side1; // top
    board side2; // left
    board side3; // front
    board side4; // right
    board side5; //bottom
    board side6; //back
};

// Each prisoner has a (x, y) position
struct position { /// x = side, y, z = r,c
    int x;
    int y;
    int z;
};
struct key {
    char name;
    int collected; 
    position pos; 
};

position prisoner_positions[3];
position wardens[2];
key keys[6];
const char KEY_LABELS[6] = {'r', 'g', 'b', 'd', 'p', 'y'};

void initialize_player_positions() {
    // side1 = TOP face (based on your definition)
    prisoner_positions[0] = (position){TOP, 1, 1}; // Top-left of top
    prisoner_positions[1] = (position){FRONT, 1, 1}; // Center of top
    prisoner_positions[2] = (position){RIGHT, 1, 1}; // Bottom-right of top
    wardens[0] = (position){LEFT, 1, 1}; 
    wardens[1] = (position){BOTTOM, 1, 1};
}

void initialize_key_positions() {
    for (int i = 0; i < 6; ++i) {
        int row, col;
        do {
            row = rand() % 3;
            col = rand() % 3;
        } while (row == 1 && col == 1); // Avoid center

        keys[i].name = KEY_LABELS[i];
        keys[i].collected = 0;
        keys[i].pos.x = i;     // Face ID
        keys[i].pos.y = row;
        keys[i].pos.z = col;
    }
}

// Don't reset structure of cube — just clear visuals
void clear_cube_visuals(cube* c) {
    board* sides[] = {&c->side1, &c->side2, &c->side3, &c->side4, &c->side5, &c->side6};
    for (int s = 0; s < 6; ++s) {
        board* b = sides[s];
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                b->tiles[i][j].content = 0;
    }
}



void initialize_board(board* b) {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            b->tiles[i][j].content = 0;
}
void print_row(tile row[3]) {
    for (int j = 0; j < 3; ++j) {
        char c = row[j].content;
        if (c == 0) c = '.';
        printf("%c ", c);
    }
}
void print_cube(cube* c) {
    printf("------ CUBE NET ------\n");

    // Print TOP face
    printf("         (top)\n");
    for (int i = 0; i < 3; ++i) {
        printf("         "); // Indent for T shape
        print_row(c->side1.tiles[i]);
        printf("\n");
    }
    printf("(left) (front) (right)\n");
    // Print LEFT, FRONT, RIGHT faces side by side
    for (int i = 0; i < 3; ++i) {
        print_row(c->side2.tiles[i]); // LEFT
        printf("  ");
        print_row(c->side3.tiles[i]); // FRONT
        printf("  ");
        print_row(c->side4.tiles[i]); // RIGHT
        printf("\n");
    }
    

    // Print BOTTOM face
    printf("         (bottom)\n");
    for (int i = 0; i < 3; ++i) {
        printf("         "); // Indent for alignment
        print_row(c->side5.tiles[i]);
        printf("\n");
    }

    // Print BACK face
    printf("         (back)\n");
    for (int i = 0; i < 3; ++i) {
        printf("         "); // Indent for alignment
        print_row(c->side6.tiles[i]);
        printf("\n");
    }

    printf("----------------------\n");
}



void initialize_cube(cube* c) {
    initialize_board(&c->side1);
    initialize_board(&c->side2);
    initialize_board(&c->side3);

    initialize_board(&c->side4);
    initialize_board(&c->side5);
    initialize_board(&c->side6);
}

void update_cube_with_players(cube* c) {
    // First clear all tiles
    clear_cube_visuals(c);// Resets all content to '.'

    // Then place players based on their position
    for (int i = 0; i < 3; ++i) {
        if (prisoners_in_jail[i]) {
            printf("prisoner in jail..press enter");
            std::cin.get();
            continue; // Skip jailed prisoners
        }
        position p = prisoner_positions[i];
        char label = '1' + i; // '1', '2', '3' for Prisoner 1–3

        board* face = nullptr;
        switch (p.x) {
            case TOP: face = &c->side1; break;
            case LEFT: face = &c->side2; break;
            case FRONT: face = &c->side3; break;
            case RIGHT: face = &c->side4; break;
            case BOTTOM: face = &c->side5; break;
            case BACK: face = &c->side6; break;
        }

        if (face != nullptr && p.y >= 0 && p.y < 3 && p.z >= 0 && p.z < 3) {
            face->tiles[p.y][p.z].content = label;
        }
    }
    for (int i = 0; i< 2; ++i)
    {
        position p = wardens[i];
        char label = i ==0 ? 'W': 'w'; 
        board* face = nullptr;
        switch (p.x) {
            case TOP: face = &c->side1; break;
            case LEFT: face = &c->side2; break;
            case FRONT: face = &c->side3; break;
            case RIGHT: face = &c->side4; break;
            case BOTTOM: face = &c->side5; break;
            case BACK: face = &c->side6; break;
        }
        if (face != nullptr && p.y >= 0 && p.y < 3 && p.z >= 0 && p.z < 3) {
            face->tiles[p.y][p.z].content = label;
        }




    }
        // Place keys on the board
    for (int i = 0; i < 6; ++i) {
        if (keys[i].collected) continue;

        position k = keys[i].pos;
        board* face = nullptr;
        switch (k.x) {
            case TOP: face = &c->side1; break;
            case LEFT: face = &c->side2; break;
            case FRONT: face = &c->side3; break;
            case RIGHT: face = &c->side4; break;
            case BOTTOM: face = &c->side5; break;
            case BACK: face = &c->side6; break;
        }

        if (face != nullptr && face->tiles[k.y][k.z].content == 0) {
            face->tiles[k.y][k.z].content = keys[i].name;
        }
    }

}



string ROTATION_MAP[18] = {
    "+X1", "+X2", "+X3", "+Y1", "+Y2", "+Y3",
    "+Z1", "+Z2", "+Z3", "-X1", "-X2", "-X3",
    "-Y1", "-Y2", "-Y3", "-Z1", "-Z2", "-Z3"
};


void rotate_row(board &b, int row) {
    tile temp = b.tiles[row][0];
    b.tiles[row][0] = b.tiles[row][2];
    b.tiles[row][2] = temp;
}

void rotate_face_clockwise(board &b) {
    tile temp[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            temp[j][2 - i] = b.tiles[i][j];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            b.tiles[i][j] = temp[i][j];
}

void rotate_face_counterclockwise(board &b) {
    tile temp[3][3];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            temp[2 - j][i] = b.tiles[i][j];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            b.tiles[i][j] = temp[i][j];
}

void rotate_position(position &p, const string &cmd) {
    int y = p.y, z = p.z;

    if (cmd[1] == 'X') {
        int col = cmd[2] - '1';
        if (p.z != col) return;

        // face ring: TOP → FRONT → BOTTOM → BACK
        int next_faces[] = {TOP, FRONT, BOTTOM, BACK};
        int dir = (cmd[0] == '+') ? 1 : -1;
        for (int i = 0; i < 4; ++i) {
            if (p.x == next_faces[i]) {
                int new_face = next_faces[(i + dir + 4) % 4];
                if (p.x == BACK || p.x == TOP)
                    p.y = 2 - p.y;
                if (p.x == BACK) z = 2 - col;
                p.x = new_face;
                return;
            }
        }
    }

    if (cmd[1] == 'Y') {
        int row = cmd[2] - '1';
        if (p.y != row) return;

        // face ring: LEFT → FRONT → RIGHT → BACK
        int next_faces[] = {LEFT, FRONT, RIGHT, BACK};
        int dir = (cmd[0] == '+') ? 1 : -1;
        for (int i = 0; i < 4; ++i) {
            if (p.x == next_faces[i]) {
                int new_face = next_faces[(i + dir + 4) % 4];
                if (p.x == BACK || p.x == LEFT)
                    p.z = 2 - p.z;
                if (p.x == BACK) p.y = 2 - row;
                p.x = new_face;
                return;
            }
        }
    }

    if (cmd[1] == 'Z') {
        int col = cmd[2] - '1';
        if (p.z != col) return;

        // face ring: TOP → LEFT → BOTTOM → RIGHT
        int next_faces[] = {TOP, LEFT, BOTTOM, RIGHT};
        int dir = (cmd[0] == '+') ? 1 : -1;
        for (int i = 0; i < 4; ++i) {
            if (p.x == next_faces[i]) {
                int new_face = next_faces[(i + dir + 4) % 4];
                if (p.x == LEFT || p.x == RIGHT)
                    p.y = 2 - p.y;
                if (p.x == BOTTOM) p.z = 2 - p.z;
                p.x = new_face;
                return;
            }
        }
    }
}

void apply_rotation(int rot, cube& c) {
    string cmd = ROTATION_MAP[rot];
    //printf("HEREEE---ROTATION %s \n", cmd.c_str());

    // <<< existing cube rotation logic >>>

    // 🔄 Now update positions:
    for (int i = 0; i < 3; ++i)
        if (!prisoners_in_jail[i])
            rotate_position(prisoner_positions[i], cmd);

    for (int i = 0; i < 2; ++i)
        rotate_position(wardens[i], cmd);

    for (int i = 0; i < 6; ++i)
        if (!keys[i].collected)
            rotate_position(keys[i].pos, cmd);
}





void show_status() {
    cout << "\n=== GAME STATUS ===" << endl;
    cout << "Keys Collected: " << keys_collected.size() << " / 6" << endl;
    cout << "Key List: ";
    for (std::vector<std::string>::iterator it = keys_collected.begin(); it != keys_collected.end(); ++it)
        cout << *it << " ";
    cout << "\nPrisoners in Jail: ";
    for (int i = 0; i < 3; ++i)
        cout << "[P" << i+1 << ": " << (prisoners_in_jail[i] ? "JAILED" : "FREE") << "] ";
    cout << "\nBlack Tiles: ";
    cout << "\nRotation Control Unlocked: " << (rotation_control_unlocked ? "Yes" : "No") << endl;
    cout << "====================\n" << endl;
}


void roll_rotation(cube& c) {
    int roll = rand() % 18;
    cout << "[Rotation] Rubik's Cube rotates on: " << ROTATION_MAP[roll] << endl;
    apply_rotation(roll, c);
}

void combat(int prisoner_index) {
    printf("Prisoner and Guard are engaging in combat…");
    int random_num = rand() % 3;
    if (random_num <= -1) {
        cout << "Combat Result: Guard loses. The Prisoner has won the fight, and escapes the Warden’s clutches! Stunned for this and next 1." << endl;
    } else {
        cout << "Combat Result: The Guard has won the fight, and prisoner " << prisoner_index + 1 << " loses and was sent to Jail!" << endl;
        prisoners_in_jail[prisoner_index] = true;
    }
}
void collect_key(const string& key_id) {
    for (int i = 0; i < 6; ++i) {
        if (!keys[i].collected && string(1, keys[i].name) == key_id) {
            keys[i].collected = 1;
            keys_collected.push_back(key_id);
            cout << "Congratulations! You have collected the " << key_id << " key.\n";
            return;
        }
    }
    cout << "No uncollected key with that name found.\n";
}

position transition_face(int face, const string& dir, int row, int col) {
    if (face == TOP) {
        if (dir == "up")    return (position){BACK, 2, col};
        if (dir == "down")  return (position){FRONT, 0, col};
        if (dir == "left")  return (position){LEFT, 0, row};
        if (dir == "right") return (position){RIGHT, 0, row};
    }
    if (face == BOTTOM) {
        if (dir == "up")    return (position){FRONT, 2, col};
        if (dir == "down")  return (position){BACK, 0, col};
        if (dir == "left")  return (position){LEFT, 2, 2-row};
        if (dir == "right") return (position){RIGHT, 2,  2 - row};
    }
    if (face == LEFT) {
        if (dir == "up")    return (position){TOP, col, 0};
        if (dir == "down")  return (position){BOTTOM, 2-col, 0};
        if (dir == "left")  return (position){BACK, 2-row, 0};
        if (dir == "right") return (position){FRONT, row, 0};
    }
    if (face == RIGHT) {
        if (dir == "up")    return (position){TOP, 2 - col, 2};
        if (dir == "down")  return (position){BOTTOM, 2, col};
        if (dir == "left")  return (position){FRONT, row, 2};
        if (dir == "right") return (position){BACK, 2-row, 2};
    }
    if (face == FRONT) {
        if (dir == "up")    return (position){TOP, 2, col};
        if (dir == "down")  return (position){BOTTOM, 0, col};
        if (dir == "left")  return (position){LEFT, row, 2};
        if (dir == "right") return (position){RIGHT, row, 0};
    }
    if (face == BACK) {
        if (dir == "up")    return (position){BOTTOM, 2, col};
        if (dir == "down")  return (position){TOP, 0, col};
        if (dir == "left")  return (position){LEFT, 2-row, 0};
        if (dir == "right") return (position){RIGHT, 2-row, 2};
    }
    return (position){face, row, col}; // fallback
}
void move_single(position* pos, const string& dir) {
    int face = pos->x;
    int y = pos->y;
    int z = pos->z;
   // printf("start %d, %d \n", y, z);

    if (dir == "up") {
        if (y > 0)
            pos->y -= 1;
        else
            *pos = transition_face(face, dir, y, z);
    }
    else if (dir == "down") {
        if (y < 2)
            pos->y += 1;
        else
            *pos = transition_face(face, dir, y, z);
    }
    else if (dir == "left") {
        if (z > 0)
            pos->z -= 1;
        else
            *pos = transition_face(face, dir, y, z);
    }
    else if (dir == "right") {
        printf("here");
        if (z < 2)
            pos->z += 1;
        else
            *pos = transition_face(face, dir, y, z);
    }
    else if (dir == "nothing") {
        return; // d noot3hing
    }
    else {
        cout << "Invalid direction: " << dir << endl;
    }
    //printf("end %d, %d \n", pos->y, pos->z);
}
void move_prisoner(int idx, const string& dir1, const string& dir2) {
    move_single(&prisoner_positions[idx], dir1);
    move_single(&prisoner_positions[idx], dir2);
}
void move_warden(int idx, const string& dir1, const string& dir2) {
    move_single(&wardens[idx], dir1);
    move_single(&wardens[idx], dir2);
}



void rescue_prisoner() {
    bool found = false;
    for (int i = 0; i < 3; ++i) {
        if (prisoners_in_jail[i] && !keys_collected.empty()) {
            prisoners_in_jail[i] = false;
            string lost_key = keys_collected.back();
            keys_collected.pop_back();
            cout << "Prisoner " << i + 1 << " rescued. Key " << lost_key << " returned randomly." << endl;
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "No jailed prisoner or no key available." << endl;
    }
}
void first_turn() {
    printf("\tThe Twisted Prison will not let you simply walk over and pick up a key. \n");
    printf("\n");
    printf("\tA random rotation of the cube will take place after a Prisoner has moved. \n");
    printf("\tAfter every rotation, the Warden can move their guards to catch prisoners. \n");
    printf("press enter to continue..\n");
    std::cin.get();
    printf("\n");
    printf("Warden, you can select a guard and move it to an adjacent tile. \n");
    printf("\tIf you land on the same tile as a Prisoner, you will engage in combat! \n");
    printf("\tWin the combat, and you will send the Prisoner to Jail. \n"); 
    printf("press enter to continue..\n"); 
    std::cin.get(); 
    printf("\n"); 
    printf("\tYour objective is to have all 3 Prisoners in Jail simultaneously. \n");
    printf("\tWarden, it’s then your turn to move your guards. \n");
    printf("\tNow a different Prisoner gets to move, and the cycle repeats again \n");
    printf("\tRemember, the prisoner moves, the cube rotates, and the warden gets to move their guards! \n");
    printf("---------------------------------\n");
    printf("Good luck! \n");
    printf("press enter to continue..\n");
    std::cin.get();
}

void take_turn(cube& game_cube) {
    for (int i = 0; i < 3; ++i) {
        if (prisoners_in_jail[i]) {
            cout << "Prisoner " << i + 1 << "'s turn skipped (in jail)." << endl;
        } else {
            update_cube_with_players(&game_cube);
            print_cube(&game_cube);
            cout << "Prisoner " << i + 1 << " it’s your move!" << endl; 
            cout << "Enter your two movement steps (up/down/left/right/nothing):\n";
            string move1, move2;
            cout << "Step 1: "; getline(cin, move1);
            cout << "Step 2: "; getline(cin, move2);
            move_prisoner(i, move1, move2);
            cout << "Enter 'k' to collect key, 'c' for combat, 'r' to rescue or Enter to skip: ";
            string action;
            getline(cin, action);

            if (action == "k") {
                cout << "Enter key tile color: ";
                string color;
                getline(cin, color);
                collect_key(color);
            } else if (action == "c") {
                combat(i);
            } else if (action == "r"){
                rescue_prisoner();
            } else {
                cout << "Prisoner passed their move.\n";
            }
        }

        roll_rotation(game_cube);
        update_cube_with_players(&game_cube);
        print_cube(&game_cube);
        cout << "Warden, mobilize your guards!" << endl;

        int chosen_guard = -1;
        string dir1, dir2, guard1, guard2;
        int guard_idx1 = -1, guard_idx2 = -1;

        cout << "Warden Move Phase:\n";
        cout << "For each of the 2 steps, choose which guard (W/w) to move and the direction (up/down/left/right/nothing).\n";

        cout << "Step 1 - Which guard (W or w)? "; getline(cin, guard1);
        cout << "Direction for Step 1: "; getline(cin, dir1);

        cout << "Step 2 - Which guard (W or w)? "; getline(cin, guard2);
        cout << "Direction for Step 2: "; getline(cin, dir2);

        // Map guard input to index
        if (guard1 == "W") guard_idx1 = 0;
        else if (guard1 == "w") guard_idx1 = 1;

        if (guard2 == "W") guard_idx2 = 0;
        else if (guard2 == "w") guard_idx2 = 1;

        // Store old positions
        position prev_pos1 = wardens[guard_idx1];
        position prev_pos2 = wardens[guard_idx2];

        // Move wardens
        if (guard_idx1 != -1) move_single(&wardens[guard_idx1], dir1);
        if (guard_idx2 != -1) move_single(&wardens[guard_idx2], dir2);

        // Check for combat (can happen on either warden)
        bool combat_occurred = false;
        for (int w = 0; w < 2; ++w) {
            for (int p = 0; p < 3; ++p) {
                if (prisoners_in_jail[p]) continue;
                if (prisoner_positions[p].x == wardens[w].x &&
                    prisoner_positions[p].y == wardens[w].y &&
                    prisoner_positions[p].z == wardens[w].z) {
                
                    cout << "⚔️  Warden " << (w == 0 ? "W" : "w") 
                        << " has encountered Prisoner " << p + 1 << "! Combat begins...\n";

                    int roll = rand() % 18 + 1;
                    if (roll <= 3) {
                        cout << "Combat Result: Guard loses. The Prisoner escapes!\n";
                        if (w == guard_idx1) wardens[w] = prev_pos1;
                        else if (w == guard_idx2) wardens[w] = prev_pos2;
                    } else {
                        cout << "Combat Result: Guard wins! Prisoner " << p + 1 << " is sent to Jail!\n";
                        prisoners_in_jail[p] = true;
                    }

                        combat_occurred = true;
                        break;
                    }
                }
            if (combat_occurred) break;
        }

        if (!combat_occurred) {
            cout << "🕵️  No prisoners encountered by the Warden this turn.\n";
        }




        // Win conditions
        int jailed_count = 0;
        for (size_t i = 0; i < prisoners_in_jail.size(); ++i) {
            if (prisoners_in_jail[i]) {
                jailed_count++;
            }
        }

        if (jailed_count == 3) {
            cout << "All Prisoners are in jail. Warden wins!" << endl;
            exit(0);
        }
        if (keys_collected.size() >= 6) {
            cout << "All 6 keys collected. Prisoners win!" << endl;
            exit(0);
        }
    }
}



int main() {
    printf("---------------------------------\n");
    printf("Welcome to the Twisted Prison… \n");
    printf("---------------------------------\n");
    printf("press enter to continue..");
    std::cin.get();
    printf("\n");
    printf("\t To our Prisoners, your one chance of escape is to work together to collect all six keys across this cube.\n");
    printf("\t When it’s your turn, you can move to any one of the four tiles that are adjacent to you. \n");
    printf("\n");
    printf("\npress enter to continue..\n");
    printf("---------------------------------\n");
    printf("\n");
    std::cin.get();

    printf("---------------------------------\n");
    printf("Prisoner 1, it’s then your turn. \n");
    printf("\n");
    printf("press enter to continue..");
    printf("\n");
    std::cin.get();
    cube game_cube;
    initialize_cube(&game_cube);
    initialize_player_positions();
    initialize_key_positions();
    game_cube.side1.tiles[1][1].content = 'P';
    srand(time(0));
    first_turn();
    while (true) {
        show_status();
        update_cube_with_players(&game_cube);
        print_cube(&game_cube);
        cout << "Choose an action:" << endl;
        cout << "1. Play next round" << endl;
        cout << "2. Show status" << endl;
        cout << "3. Quit" << endl;

        string input;
        getline(cin, input);

        if (input == "1") {
            take_turn(game_cube);
        } else if (input == "2") {
            show_status();
        } else if (input == "3") {
            cout << "Goodbye!" << endl;
            break;
        } else {
            cout << "Invalid choice. Try again." << endl;
        }
    }

    return 0;
}
