import json
import os.path
import pdb
import pickle

import mdl_ipam

from src.utils.common import *
from src.utils.utils import new_dir, GlobalConstrains, load_patterns, load_sequences


class MineAlgInterface:
    def __init__(self, token='', dataset_name='', matches=None, store_dir='', player='', attributes=None):
        if attributes is None:
            attributes = []
        if matches is None:
            matches = []
        self.token = token
        self.dataset_name = dataset_name
        self.matches = matches
        self.store_dir = store_dir
        self.player = player
        self.attributes = attributes
        new_dir(os.path.join(DATA_DIR, store_dir))

        self.version = 0
        self.store_path = os.path.join(DATA_DIR, store_dir, str(self.get_version()))

    def get_version(self):
        return self.version

    def reset_version(self):
        self.version = 0

    def set_version(self, version):
        self.version = version

    def set_new_store_path(self, version='latest'):
        if version == 'latest':
            self.store_path = os.path.join(DATA_DIR, self.store_dir, str(self.get_version()))
        elif version.isdigit():
            self.store_path = os.path.join(DATA_DIR, self.store_dir, version)
        else:
            raise NameError(f'version({version}) is not str. cannot be set on store path.')

    def get_store_path(self, version='latest'):
        if version == 'latest':
            return os.path.join(DATA_DIR, self.store_dir, str(self.get_version()))
        elif version.isdigit():
            return os.path.join(DATA_DIR, self.store_dir, version)
        else:
            raise NameError(f'version({version}) is not str. cannot be set on store path.')

    def data_preprocess(self):
        input_filenames = ' '.join([match + '.json' for match in self.matches])
        if self.dataset_name.startswith('Table Tennis'):
            file_type = FileType.TableTennis
        elif self.dataset_name.startswith('Badminton'):
            file_type = FileType.Badminton
        elif self.dataset_name.startswith('Tennis'):
            file_type = FileType.Tennis
        else:
            file_type = FileType.NotDefine

        if file_type == FileType.NotDefine:
            raise NotImplementedError('This dataset is not existed.')

        input_dir = os.path.join(DATASET_DIR, self.dataset_name)

        new_dir(os.path.join(self.store_path, INTERMEDIATE_DIR))
        new_dir(os.path.join(self.store_path, OUTPUT_DIR))
        output_filename = os.path.join(self.store_path, INTERMEDIATE_DIR, INTERMEDIATE_SEQUENCE)
        sequence_filename = os.path.join(self.store_path, OUTPUT_DIR, SEQUENCE_FILE)

        attributes_names_str = ' '.join([attribute.replace(' ', '') for attribute in self.attributes])
        mdl_ipam.load_data(file_type.value,
                           self.player,
                           input_filenames,
                           input_dir,
                           output_filename,
                           sequence_filename,
                           attributes_names_str
                           )

    def run(self, constrains: GlobalConstrains, insert_tactics=None):

        if insert_tactics is None:
            insert_tactics = []
        attr_use_str = ' '.join([str(u) for u in constrains.attr_use])

        intermediate_filename = os.path.join(self.store_path, INTERMEDIATE_DIR, INTERMEDIATE_SEQUENCE)

        output_dir = os.path.join(self.store_path, OUTPUT_DIR)
        new_dir(output_dir)

        insert_tactics_filename = os.path.join(self.store_path, INTERMEDIATE_DIR, INTERMEDIATE_INSERT_TACTICS)
        with open(insert_tactics_filename, 'w') as f:
            f.write(json.dumps(insert_tactics, separators=(',', ':')))

        mdl_ipam.run(False, False, intermediate_filename, "", output_dir, constrains.minsup,
                     constrains.index_min, constrains.index_max,
                     constrains.length_min, constrains.length_max,
                     attr_use_str,
                     PATTERN_FILE,
                     insert_tactics_filename)
        print('PYTHON: finish run mdl.')

    def modify_and_run(self, delete_tactics_id, insert_tactics, last_tactics_filename, constrains: GlobalConstrains):

        attr_use_str = ' '.join([str(u) for u in constrains.attr_use])

        intermediate_filename = os.path.join(self.store_path, INTERMEDIATE_DIR, INTERMEDIATE_SEQUENCE)

        output_dir = os.path.join(self.store_path, OUTPUT_DIR)
        new_dir(output_dir)

        delete_tactics_id_str = ' '.join([str(tactic_id) for tactic_id in delete_tactics_id])

        insert_tactics_filename = os.path.join(self.store_path, INTERMEDIATE_DIR, INTERMEDIATE_INSERT_TACTICS)
        with open(insert_tactics_filename, 'w') as f:
            f.write(json.dumps(insert_tactics, separators=(',', ':')))

        mdl_ipam.modify_and_run(False, False, intermediate_filename, "", output_dir, constrains.minsup,
                                constrains.index_min, constrains.index_max,
                                constrains.length_min, constrains.length_max,
                                attr_use_str,
                                PATTERN_FILE,
                                last_tactics_filename,
                                delete_tactics_id_str,
                                insert_tactics_filename)
        print('PYTHON: finish modify and run mdl.')

    def update_version(self):
        self.version += 1

    def get_tactics(self):
        return load_patterns(os.path.join(self.store_path, OUTPUT_DIR, PATTERN_FILE))

    def get_rallies(self):
        return load_sequences(os.path.join(self.store_path, OUTPUT_DIR, SEQUENCE_FILE))

    def save(self, filepath):
        """
        Saves the model as a pickle to the given filepath.
        """
        model_data = {
            'model_type': 'MineAlgInterface',
            'version': self.version,
            'params': {
                'token': self.token,
                'dataset_name': self.dataset_name,
                'player': self.player,
                'matches': self.matches,
                'store_dir': self.store_dir,
                'attributes': self.attributes,
            }
        }
        with open(filepath, 'wb') as file:
            pickle.dump(model_data, file)

    @classmethod
    def load(cls, filepath, params=None):
        """
        Loads a model from a pickle file at the given filepath.
        """
        # filepath = os.path.join(DATA_DIR, store_dir, store_dir + '.pkl')
        assert os.path.isfile(filepath) or params is not None, 'One of filepath and params need to be available.'
        if not os.path.isfile(filepath):
            model = cls(**params)
            return model

        with open(filepath, 'rb') as file:
            model_data = pickle.load(file)
        assert model_data['model_type'] == 'MineAlgInterface', 'Invalid model type for MineAlgInterface'
        model = cls(**model_data['params'])
        model.version = model_data['version']
        return model
