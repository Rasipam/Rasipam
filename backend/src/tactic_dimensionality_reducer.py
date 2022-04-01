import os
import pickle
import warnings
import numpy as np

from difflib import SequenceMatcher
from joblib import dump, load
from sklearn.decomposition import PCA

from src.utils.common import NR_ATTR


def edit_cost(p1, p2):
    sequence_matcher = SequenceMatcher(None, p1, p2)
    matching_blocks = sequence_matcher.get_matching_blocks()
    ans = len(p1) + len(p2) - sum([m.size for m in matching_blocks]) * 2
    return ans


def normalize(coordinate, x_range=0.8, y_range=0.8):
    normalized_coordinate = np.empty(coordinate.shape)
    for i in range(coordinate.shape[1]):
        normalized_coordinate[:, i] = (coordinate[:, i] - coordinate[:, i].min()) / \
                                      (coordinate[:, i].max() - coordinate[:, i].min()) * x_range + (1 - x_range) / 2
    print(normalized_coordinate)
    return normalized_coordinate


class TacticDimensionalityReducer:
    def __init__(self):
        self.pca = None
        self.pca_1 = None
        self.base_tactics = None

    def _get_mapping(self, tactics):
        tactics = [tactic["hits"] for tactic in tactics["patterns"]]
        base_tactics = [tactic["hits"] for tactic in self.base_tactics["patterns"]]
        mapping = [[0 for _ in base_tactics] for _ in tactics]
        for i in range(len(mapping)):
            for j in range(len(base_tactics)):
                mapping[i][j] = sum([edit_cost([hit[a_id] for hit in tactics[i]],
                                               [hit[a_id] for hit in base_tactics[j]])
                                     for a_id in range(NR_ATTR)])
        return mapping

    def fit(self, tactics, **kwargs):
        if len(tactics['patterns']) == 0:
            return []
        if self.pca is not None:
            warnings.warn("Overwriting exist pca model...")
        self.pca = PCA(n_components=2, random_state=7, **kwargs)
        self.pca_1 = PCA(n_components=1, random_state=7, **kwargs)
        self.base_tactics = tactics
        mapping = self._get_mapping(tactics)
        self.pca.fit(np.array(mapping))
        self.pca_1.fit(np.array(mapping))

    def transform(self, tactics):
        if len(tactics['patterns']) == 0:
            return []
        if self.pca is None:
            raise RuntimeError("No model fitted before transform.")
        mapping = self._get_mapping(tactics)
        coord2 = normalize(self.pca.transform(np.array(mapping)))
        coord1 = normalize(self.pca_1.transform(np.array(mapping)))
        return np.concatenate([coord2, coord1], axis=1).tolist()

    def fit_transform(self, tactics, **kwargs):
        if len(tactics['patterns']) == 0:
            return []
        if self.pca is not None:
            warnings.warn("Overwriting exist pca model...")
        self.pca = PCA(n_components=2, random_state=7, **kwargs)
        self.pca_1 = PCA(n_components=1, random_state=7, **kwargs)
        self.base_tactics = tactics
        mapping = self._get_mapping(tactics)
        coord2 = normalize(self.pca.fit_transform(np.array(mapping)))
        coord1 = normalize(self.pca_1.fit_transform(np.array(mapping)))
        return np.concatenate([coord2, coord1], axis=1).tolist()

    def save(self, out_dir):
        save_path = os.path.join(out_dir, 'tactic_dim_reducer.pkl')

        model_data = {
            'pca_2': self.pca,
            'pca_1': self.pca_1,
            'base_tactics': self.base_tactics
        }
        # base_tactics_path = os.path.join(out_dir, 'tactic_dim_reducer_base_tactics.bin')
        print('Saving PCA...')
        with open(save_path, 'wb') as f:
            pickle.dump(model_data, f)

    @classmethod
    def load(cls, out_dir):
        save_path = os.path.join(out_dir, 'tactic_dim_reducer.pkl')
        # base_tactics_path = os.path.join(out_dir, 'tactic_dim_reducer_base_tactics.bin')
        model = cls()

        if os.path.isfile(save_path):
            with open(save_path, 'rb') as file:
                model_data = pickle.load(file)
            model.pca = model_data['pca_2']
            model.base_tactics = model_data['base_tactics']
            model.pca_1 = model_data['pca_1']
        return model
