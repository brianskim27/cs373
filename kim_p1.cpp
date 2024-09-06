#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;

// Structure to define transitions
struct Transition {
    int nextState;
    char writeChar;
    char moveDirection; // L - back, S - stay, R - next
};

// Turing Machine simulator
class turingMachine {
    unordered_map<int, unordered_map<char, Transition>> transitions;    // store the transitions of the machine
    set<int> acceptStates;  // set of all accept states
    set<int> rejectStates;  // set of all reject states
    int startState; // starting state of the machine
    string tape;    // Tape (input string)
    int maxTransitions; // Maximum number of transitions allowed
    int headPosition;   // Current position of the tape head
    int currentState;   // Current state of machine
    const char BLANK = ' '; // Blank space

public:
    turingMachine(const string& filename, const string& input, int maxTransitions) : tape(input), maxTransitions(maxTransitions), headPosition(0) {
        tape = input;   // Initialize tape with input string
        parseFile(filename);
        currentState = startState;
    }

    // Parse state lines
    void parseState(const string& line) {
        stringstream ss(line);
        string state, status;
        int position;

        ss >> state >> position >> status;

        if(status == "start") startState = position;
        else if(status == "accept") acceptStates.insert(position);
        else if(status == "reject") rejectStates.insert(position);
    }

    // Parse transition lines
    void parseTransition(const string& line) {
        stringstream ss(line);
        string temp, moveDir;
        int q, r;
        char a, b;

        ss >> temp >> q >> a >> r >> b >> moveDir;

        // Replace underscore with a blank space
        if(a == '_') a = BLANK;
        if(b == '_') b = BLANK;
        
        transitions[q][a] = {r, b, moveDir[0]};
    }

    // Parse machine definition file
    void parseFile(const string& filename) {
        ifstream file(filename);
        string line;

        // Parse the states and transitions
        while(getline(file, line)) {
            if(line.substr(0, 5) == "state") parseState(line);
            else if (line.substr(0, 10) == "transition") parseTransition(line);
        }
    }

    // Main function to simulate the Turing machine
    void run() {
        int transitionsCount = 0;

        while(transitionsCount < maxTransitions) {
            // Extend tape to the right with blanks if the head moves beyond the current tape size
            if(headPosition >= tape.size()) tape += BLANK;
            
            char currentChar = headPosition < tape.size() ? tape[headPosition] : ' ';   // Set the current char to the current position of head (beyond tape bounds => blank space)

            // Check if there is a valid transition
            if(transitions[currentState].find(currentChar) == transitions[currentState].end()) {
                cout << tape << " reject\n";
                return;
            }

            // Perform the transition
            Transition t = transitions[currentState][currentChar];
            tape[headPosition] = t.writeChar;
            currentState = t.nextState;

            // Move the head
            if(t.moveDirection == 'R') headPosition++;  // move to the next symbol
            else if(t.moveDirection == 'L') {
                headPosition--; // move back one symbol
                if(headPosition < 0) {
                    cerr << "Error: Head moved back beyond the tape bounds\n";
                    return;
                }
            }
            else if(t.moveDirection == 'S');    // Do nothing if transition is 'S'

            transitionsCount++;

            // Stop machine if transitioned to accept or reject state
            if(acceptStates.find(currentState) != acceptStates.end()) {
                cout << tape << " accept\n";
                return;
            } 
            else if(rejectStates.find(currentState) != rejectStates.end()) {
                cout << tape << " reject\n";
                return;
            }
        }

        cout << tape << " quit\n";  // Stop machine - exceeded max transitions
    }
};

int main(int argc, char* argv[]) {
    // Catch incorrect commandline inputs
    if(argc !=4) {
        cerr << "Usage: " << argv[0] << " <definition_file> <input_string> <max_transitions>\n";
        return 1;
    }

    string filename = argv[1];
    string inputString = argv[2];
    int maxTransitions = stoi(argv[3]);

    turingMachine turingMachine(filename, inputString, maxTransitions);
    turingMachine.run();

    return 0;
}