#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unordered_set>

using namespace std;

vector<bool> prisoners_in_jail(3, false);
vector<string> keys_collected;
unordered_set<string> black_tiles;
bool rotation_control_unlocked = false;

const int BOARD_SIZE = 5;
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
position prisoner_positions[3];
position wardens[2];
position keys[6];

void initialize_player_positions() {
    // side1 = TOP face (based on your definition)
    prisoner_positions[0] = (position){TOP, 1, 1}; // Top-left of top
    prisoner_positions[1] = (position){FRONT, 1, 1}; // Center of top
    prisoner_positions[2] = (position){RIGHT, 1, 1}; // Bottom-right of top
    wardens[0] = (position){LEFT, 1, 1}; 
    wardens[1] = (position){BOTTOM, 1, 1};
}

void initialize_key_positions() {
    for (int f = 0; f < 6; ++f) {
        int row, col;
        do {
            row = rand() % 3;
            col = rand() % 3;
        } while (row == 1 && col == 1); // Avoid center

        keys[f].x = f;    // face
        keys[f].y = row;  // row
        keys[f].z = col;  // column
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
    initialize_cube(c); // Resets all content to '.'

    // Then place players based on their position
    for (int i = 0; i < 3; ++i) {
        if (prisoners_in_jail[i]) continue; // Skip jailed prisoners

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
        position k = keys[i];
        board* face = nullptr;
        switch (k.x) {
            case TOP: face = &c->side1; break;
            case LEFT: face = &c->side2; break;
            case FRONT: face = &c->side3; break;
            case RIGHT: face = &c->side4; break;
            case BOTTOM: face = &c->side5; break;
            case BACK: face = &c->side6; break;
        }
        if (face != nullptr && k.y >= 0 && k.y < 3 && k.z >= 0 && k.z < 3) {
        // Check if the key has already been collected
        string key_id = "Key" + to_string(i);
        if (keys_collected.end() == keys_collected.begin() || 
        keys_collected.end() == find(keys_collected.begin(), keys_collected.end(), key_id)) {
        if (face->tiles[k.y][k.z].content == 0)
            face->tiles[k.y][k.z].content = 'K';
    }
}

    }


}

string ROTATION_MAP[18] = {
    "+X1", "+X2", "+X3", "+Y1", "+Y2", "+Y3",
    "+Z1", "+Z2", "+Z3", "-X1", "-X2", "-X3",
    "-Y1", "-Y2", "-Y3", "-Z1", "-Z2", "-Z3"
};

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
    for (std::unordered_set<std::string>::iterator it = black_tiles.begin(); it != black_tiles.end(); ++it)
        cout << *it << " ";
    cout << "\nRotation Control Unlocked: " << (rotation_control_unlocked ? "Yes" : "No") << endl;
    cout << "====================\n" << endl;
}


void roll_rotation() {
    int roll = rand() % 18;
    cout << "[Rotation] Rubik's Cube rotates on: " << ROTATION_MAP[roll] << endl;
}

void combat(int prisoner_index) {
    printf("Prisoner and Guard are engaging in combat…");
    int roll = rand() % 18 + 1;
    if (roll <= 9) {
        cout << "Combat Result: Guard loses. The Prisoner has won the fight, and escapes the Warden’s clutches! Stunned for this and next 1." << endl;
    } else {
        cout << "Combat Result: The Guard has won the fight, and prisoner " << prisoner_index + 1 << " loses and was sent to Jail!" << endl;
        prisoners_in_jail[prisoner_index] = true;
    }
}

void collect_key(const string& key_id) {
    keys_collected.push_back(key_id);
    cout << "Congratulations Prisoners, you have obtained " << key_id << "!" << endl;

    int key_num = keys_collected.size();
    if (key_num == 1 || key_num == 2) {
        black_tiles.insert(key_id);
    } else if (key_num == 5) {
        rotation_control_unlocked = true;
    }
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

void take_turn() {
    for (int i = 0; i < 3; ++i) {
        if (prisoners_in_jail[i]) {
            cout << "Prisoner " << i + 1 << "'s turn skipped (in jail)." << endl;
        } else {
            cout << "Prisoner " << i + 1 << " it’s your move!" << endl;
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

        roll_rotation();
        cout << "Warden, mobilize your guards!" << endl;

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
            take_turn();
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
