from enum import Enum

NR_ATTR = 3

ATTRIBUTES = {
    "Table Tennis (Male)": ["BallPosition",
                            "StrikePosition",
                            "StrikeTech"],
    "Table Tennis (Female)": ["BallPosition",
                              "StrikePosition",
                              "StrikeTech"],
    "Badminton (Male)": ["Ball Position",
                         "Ball Height",
                         "Hit Technique"],
    "Badminton (Female)": ["BallPosition",
                           "StrikePosition",
                           "StrikeTech"],
    "Tennis (Male)": ["Ball Position",
                      "Hitting Pose",
                      "Hit Technique"],
    "Tennis (Female)": ["BallPosition",
                        "StrikePosition",
                        "StrikeTech"],
}


class FileType(Enum):
    TableTennis = 0
    Badminton = 1
    Tennis = 2
    NotDefine = 3


INF = 100000000

DATA_DIR = "data"
DATASET_DIR = "datasets"

INTERMEDIATE_DIR = "intermediate"
INTERMEDIATE_SEQUENCE = "sequences.dat"
INTERMEDIATE_INSERT_TACTICS = "insert_tactics.json"

OUTPUT_DIR = "output"

PATTERN_IN_SEQUENCE_FILE = "pattern_in_sequence.dat"
PATTERN_POSITION_IN_SEQUENCE_FILE = "pattern_position_in_sequence.dat"
SEQUENCE_FILE = "sequence.json"
WINNER_FILE = "winner.dat"
PATTERN_FILE = "pattern.json"
PATTERN_ID_FILE = "pattern_id.json"

MATCH_FILE = "match.json"
SERVER_FILE = "server.dat"
