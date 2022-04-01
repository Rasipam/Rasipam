import os

from src.model import SequenceFilter
from src.utils.token import is_valid_token
from src.utils.utils import load_json, new_dir, GlobalConstrains, TimeSpot
from src.utils.common import ATTRIBUTES, DATA_DIR, DATASET_DIR


class DataManager:
    def __init__(self):
        self.datasets_path = DATASET_DIR
        self.datasets = []
        self.__reload_datasets()

        self.mdl_metadata = {}
        self.global_constrains = {}

        self.tactic_set = {}

        self.timeline = {}

        self.preference_tactics = {}

        new_dir(DATA_DIR)

    def __reload_dataset(self, folder):
        path = os.path.join(self.datasets_path, folder)
        matches = []
        attrs = ATTRIBUTES[folder]
        for file in os.listdir(path):
            if file.startswith("."):
                continue
            match_json = load_json(os.path.join(path, file))

            matches.append({
                'name': file.split('.')[0],
                'players': [[player["name"] for player in team] for team in match_json["player"]],
                'sequenceCount': sum([len(game["list"]) for game in match_json["record"]["list"]]),
            })
        return {
            "name": folder,
            "matches": matches,
            "attrs": attrs,
        }

    def __reload_datasets(self):
        for folder in os.listdir(self.datasets_path):
            if os.path.isdir(os.path.join(self.datasets_path, folder)):
                self.datasets.append(self.__reload_dataset(folder))

    def init_token(self, token):
        self.init_metadata(token)
        self.init_global_constrains(token)
        self.init_tactic_set(token)
        self.init_timeline(token)
        self.init_preference_tactics(token)

    def get_datasets(self):
        return self.datasets

    def init_global_constrains(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        self.global_constrains[token] = []

    def get_global_constrains(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        if len(self.global_constrains[token]) == 0:
            raise NameError(f'no global constrains for {token}')
        return self.global_constrains[token][-1]

    def update_global_constrains(self, token, global_constrains):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        self.global_constrains[token].append(global_constrains)

    def undo_global_constrains(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        if len(self.global_constrains[token]) == 0:
            raise NameError(f'no global constrains for {token}...cannot undo')
        self.global_constrains[token].pop()

    def init_metadata(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        self.mdl_metadata[token] = {'dataset_name': '',
                                    'matches': [],
                                    'store_dir': token,
                                    'player': '',
                                    'attributes': []}

    def update_metadata(self, token, metadata):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        self.mdl_metadata[token].update(metadata)

    def get_metadata(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        return self.mdl_metadata[token]

    def filter_matches(self, sequence_filter: SequenceFilter):
        dataset = [dataset for dataset in self.datasets if dataset['name'] == sequence_filter.dataset][0]
        metadata = {'dataset_name': sequence_filter.dataset,
                    'matches': [],
                    'player': sequence_filter.player,
                    'attributes': dataset['attrs']}
        for match in dataset['matches']:
            if (sequence_filter.player in match['players'][0] and
                len(set.intersection(set(sequence_filter.opponents), set(match['players'][1]))) > 0) \
                or \
               (sequence_filter.player in match['players'][1] and
                len(set.intersection(set(sequence_filter.opponents), set(match['players'][0]))) > 0):

                metadata['matches'].append(match['name'])
        return metadata

    def init_tactic_set(self, token):
        self.tactic_set[token] = []

    def update_tactic_set(self, token, tactic_set):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        self.tactic_set[token].append(tactic_set)

    def get_tactic_set(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        if len(self.tactic_set[token]) == 0:
            raise NameError(f'no tactic set built for {token}')
        return self.tactic_set[token][-1]

    def undo_tactic_set(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        if len(self.tactic_set[token]) == 0:
            raise NameError(f'no tactic set built for {token}...cannot undo')
        self.tactic_set[token].pop()

    def init_timeline(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        self.timeline[token] = []

    def update_timeline(self, token, time_spot: TimeSpot, mdl_version: int):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        self.timeline[token].append({'time_spot': time_spot, 'last_version': mdl_version})

    def get_timeline(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        if len(self.timeline[token]) == 0:
            raise NameError(f'no action on timeline for {token}')
        return self.timeline[token][-1]

    def undo_timeline(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        if len(self.timeline[token]) == 0:
            raise NameError(f'no action on timeline for {token}...cannot undo')
        self.timeline[token].pop()

    def init_preference_tactics(self, token):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        self.preference_tactics[token] = []

    def find_preference_tactics(self, token, new_tactic):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        return new_tactic in self.preference_tactics[token]

    def add_preference_tactics(self, token, new_tactic):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        if self.find_preference_tactics(token, new_tactic):
            raise NameError(f'the preference tactic already in preference tactics: {token}')
        self.preference_tactics[token].append(new_tactic)

    def delete_preference_tactics(self, token, tactic):
        if not is_valid_token(token):
            raise NameError(f"token is not valid: {token}")
        if not self.find_preference_tactics(token, tactic):
            raise NameError(f'the preference tactic not in preference tactics: {token}')
        self.preference_tactics[token].remove(tactic)

    def get_preference_tactics(self, token):
        return self.preference_tactics[token]


data_manager = DataManager()
