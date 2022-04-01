import os
import json
from copy import deepcopy
from enum import Enum
from pathlib import Path
from typing import List

from pydantic import BaseModel

from src.model import TacticSet
from src.utils.common import INF, NR_ATTR


def load_json(*file_paths, **kwargs):
    """
    Attempts to load a data json from the file paths given, and returns the first
    one whose file path exists.
    """
    for path in file_paths:
        if os.path.exists(path):
            with open(path, encoding='utf-8') as f:
                return json.load(f, **kwargs)
    raise FileNotFoundError(", ".join(file_paths))


def new_dir(dir_path, **kwargs):
    """
    Attempts to make path from the dir pats given, and returns if the target directory already exists.
    Any missing parents of this path are created as needed.
    """
    try:
        Path(dir_path).mkdir(parents=True)
    except FileExistsError:
        return True
    return False


class GlobalConstrains(BaseModel):
    index_min: int = 0
    index_max: int = -1
    length_min: int = 0
    length_max: int = INF
    minsup: int = 10

    attr_use: List[int] = [1] * NR_ATTR

    def set_pattern_window_index_range(self, index_min, index_max):
        self.index_min, self.index_max = index_min, index_max

    def set_pattern_length_range(self, length_min, length_max):
        self.length_min, self.length_max = length_min, length_max

    def update_pattern_length_range(self, step=1):
        self.length_max += step

    def set_minsup(self, minsup):
        self.minsup = minsup

    def set_exist_attribute(self, attr_id, exist=False):
        self.attr_use[attr_id] = int(exist)

    def set_attr_use(self, attr_id, use):
        self.attr_use[attr_id] = use


def load_sequences(file_path, **kwargs):
    """
    Load a sequence and transform it into a list.
    """
    with open(file_path, 'r', encoding='utf-8') as f:
        return json.load(f, **kwargs)


def load_patterns(file_path, **kwargs):
    with open(file_path, 'r', encoding='utf-8') as f:
        return json.load(f, **kwargs)


class TimeSpot(Enum):
    InitRun = 0
    GlobalModification = 1
    LocalModification = 2


def find_tactic_id(tactics, tactic_set, tactics_uuid):
    """
    Find the tactic id in the c++ side. Return a list of id if a list of uuid is passed.
    """
    if not isinstance(tactics_uuid, list):
        tactic_index = [i for i in range(len(tactic_set.tactics)) if tactic_set.tactics[i].id == tactics_uuid][0]
        tactic_id = tactics["patterns"][tactic_index]["id"]
    else:
        tactic_indices = [i for i in range(len(tactic_set.tactics)) if tactic_set.tactics[i].id in tactics_uuid]
        tactic_id = [tactics["patterns"][tactic_index]["id"] for tactic_index in tactic_indices]
    return tactic_id


def find_tactic(tactic_set, tactics_uuid):
    """
    Find the tactic in tactic set. Return a list of tactic if a list of uuid is passed.
    """
    if not isinstance(tactics_uuid, list):
        tactic = [tactic for tactic in tactic_set.tactics if tactic.id == tactics_uuid][0]
    else:
        tactic = [tactic for tactic in tactic_set.tactics if tactic.id in tactics_uuid]
    return tactic


def split_tactic(tactic, tactic_surrounding, attr=-1, hit=-1, top_frequent_value=3):
    insert_tactics = []
    hit_range = range(hit, hit + 1) if hit != -1 else range(len(tactic))
    for hit in hit_range:
        attr_range = range(attr, attr + 1) if attr != -1 else range(len(tactic[hit]))
        for attr in attr_range:
            if tactic[hit][attr] == "":
                frequent_value = tactic_surrounding[hit + TacticSet.surrounding_step][attr]
                frequent_value = {k: v for k, v in sorted(frequent_value.items(), key=lambda item: item[1])[::-1]}
                for value in list(frequent_value.keys())[:top_frequent_value]:
                    new_tactic = deepcopy(tactic)
                    new_tactic[hit][attr] = value
                    insert_tactics.append(new_tactic)
    return insert_tactics


def check_event_overlap(event1, event2):
    for i in range(len(event1)):
        if event1[i] == event2[i] and event1[i] != "":
            return True
    return False


def check_overlap(tactic1, tactic2, offset):
    if offset == 0 or offset == len(tactic1) + len(tactic2):
        return -1, -1
    start_pos_a = max(0, len(tactic2) - offset)
    start_pos_b = max(0, offset - len(tactic2))
    if start_pos_b == 0:
        new_length = max(start_pos_a + len(tactic1), len(tactic2))
    else:
        new_length = max(start_pos_b + len(tactic2), len(tactic1))
    begin_overlap = [-1]
    len_overlap = [0]
    for pos in range(new_length):
        if pos >= start_pos_a and pos >= start_pos_b and pos - start_pos_a < len(tactic1) and pos - start_pos_b < len(
                tactic2):
            if check_event_overlap(tactic1[pos - start_pos_a], tactic2[pos - start_pos_b]):
                if begin_overlap[-1] == -1:
                    begin_overlap[-1] = pos
                len_overlap[-1] += 1
            else:
                begin_overlap.append(-1)
                len_overlap.append(0)
    max_begin_overlap = -1
    max_len_overlap = 0
    for i, l in enumerate(len_overlap):
        if l > max_len_overlap:
            max_len_overlap = l
            max_begin_overlap = begin_overlap[i]
    return max_begin_overlap, max_len_overlap


def build_tactic(a, b, offset, begin_overlap, len_overlap):
    tactic = []
    start_pos_a = max(0, len(b) - offset)
    start_pos_b = max(0, offset - len(b))
    if start_pos_b == 0:
        new_length = max(start_pos_a + len(a), len(b))
    else:
        new_length = max(start_pos_b + len(b), len(a))
    for pos in range(begin_overlap, begin_overlap + len_overlap):
        event = ["" for _ in range(NR_ATTR)]
        # if start_pos_a <= pos < len(a) + start_pos_a:
        #     for i in range(NR_ATTR):
        #         event[i] = a[pos - start_pos_a][i]
        # if start_pos_b <= pos < len(b) + start_pos_b:
        #     for i in range(NR_ATTR):
        #         event[i] = b[pos - start_pos_b][i]
        for i in range(NR_ATTR):
            if a[pos - start_pos_a][i] == b[pos - start_pos_b][i]:
                event[i] = a[pos - start_pos_a][i]
        tactic.append(event)
    return tactic


def dfs_merge_tactics(cur_tactic, tactics, new_tactics):
    if len(tactics) == 0:
        if len(cur_tactic) > len(new_tactics):
            new_tactics = cur_tactic
        # if cur_tactic not in new_tactics:
        #     new_tactics.append(cur_tactic)
        return new_tactics
    for offset in range(0, len(cur_tactic) + len(tactics[0].tactic) + 1):
        begin_overlap, len_overlap = check_overlap(cur_tactic, tactics[0].tactic, offset)
        if len_overlap > 0:
            new_tactic = build_tactic(cur_tactic, tactics[0].tactic, offset, begin_overlap, len_overlap)
            new_tactics = dfs_merge_tactics(new_tactic, tactics[1:], new_tactics)
    return new_tactics


def merge_tactic(tactics):
    insert_tactics = dfs_merge_tactics(tactics[0].tactic, tactics[1:], [])
    return [insert_tactics]


def dfs_increment_hit(frequent_count, top_frequent_value, new_fragments, hit_id, attr_id, cur_fragment):
    if hit_id == len(frequent_count):
        for hit in cur_fragment:
            all_empty_hit = True
            for attr in hit:
                if attr != "":
                    all_empty_hit = False
            if all_empty_hit:
                return new_fragments

        new_fragments.append(cur_fragment)
        return new_fragments
    value_count = frequent_count[hit_id][attr_id]

    nex_fragment = deepcopy(cur_fragment)
    if attr_id + 1 < len(frequent_count[hit_id]):
        dfs_increment_hit(frequent_count, top_frequent_value, new_fragments, hit_id, attr_id + 1, nex_fragment)
    else:
        dfs_increment_hit(frequent_count, top_frequent_value, new_fragments, hit_id + 1, 0, nex_fragment)
    for k in list(value_count.keys())[:min(top_frequent_value, len(value_count))]:
        nex_fragment = deepcopy(cur_fragment)
        nex_fragment[hit_id][attr_id] = k
        if attr_id + 1 < len(frequent_count[hit_id]):
            dfs_increment_hit(frequent_count, top_frequent_value, new_fragments, hit_id, attr_id + 1, nex_fragment)
        else:
            dfs_increment_hit(frequent_count, top_frequent_value, new_fragments, hit_id + 1, 0, nex_fragment)
    return new_fragments


def increment_hit(tactic, rallies, direction, hit_count, top_frequent_value=2, frequency_threshold=5):
    frequent_value_count = []
    for i in range(hit_count):
        frequent_value_count.append([])
        for j in range(NR_ATTR):
            frequent_value_count[i].append({})
    for rally_id in tactic.index:
        tactic_indices = tactic.index[rally_id]
        rally = rallies["sequences"][rally_id]["events"]
        for tactic_index in tactic_indices:
            if direction == 0:
                rally_hit_range = range(max(0, tactic_index - hit_count), tactic_index)
                count_offset = tactic_index - hit_count
            else:
                rally_hit_range = range(tactic_index + len(tactic.tactic),
                                        min(tactic_index + len(tactic.tactic) + hit_count, len(rally)))
                count_offset = tactic_index + len(tactic.tactic)
            for hit_id in rally_hit_range:
                for attr_id in range(len(rally[hit_id])):
                    if rally[hit_id][attr_id] not in frequent_value_count[hit_id - count_offset][attr_id]:
                        frequent_value_count[hit_id - count_offset][attr_id][rally[hit_id][attr_id]] = 0
                    frequent_value_count[hit_id - count_offset][attr_id][rally[hit_id][attr_id]] += 1

    for i in range(hit_count):
        for j in range(NR_ATTR):
            frequent_value = frequent_value_count[i][j]
            frequent_value = {k: v for k, v in sorted(frequent_value.items(), key=lambda item: item[1])[::-1]}
            # if v > frequency_threshold}
            frequent_value_count[i][j] = frequent_value
    print(frequent_value_count)

    increment_fragments = [[list(freq_value.keys())[0] if i == 0 else "" for i, freq_value in enumerate(freq_attr)]
                           for freq_attr in frequent_value_count]
    # dfs_increment_hit(frequent_value_count, top_frequent_value, [], 0, 0, [[""] * NR_ATTR] * hit_count)
    insert_tactics = [increment_fragments + deepcopy(tactic.tactic) if direction == 0 else \
                          deepcopy(tactic.tactic) + increment_fragments]
    # [frag + deepcopy(tactic.tactic) if direction == 0 else
    #               deepcopy(tactic.tactic) + frag for frag in increment_fragments]
    return insert_tactics


def decrement_hit(tactic, direction, hit_count):
    if hit_count > len(tactic.tactic):
        return []

    insert_tactic = [deepcopy(tactic.tactic[hit_count:] if direction == 0 else tactic.tactic[:-hit_count])]
    return insert_tactic


def modify_value(tactic, rallies, hit_id, attr_id, modification_type, target_value=''):
    if modification_type == 'Replace':
        insert_tactic = deepcopy(tactic.tactic)
        insert_tactic[hit_id][attr_id] = target_value
        return [insert_tactic]
    if modification_type == 'Ignore':
        insert_tactic = deepcopy(tactic.tactic)
        insert_tactic[hit_id][attr_id] = ''
        return [insert_tactic]
    if modification_type == 'Explore':
        frequent_value_count = {}
        for rally_id in tactic.index:
            tactic_indices = tactic.index[rally_id]
            rally = rallies["sequences"][rally_id]["events"]
            for tactic_index in tactic_indices:
                if rally[tactic_index + hit_id][attr_id] not in frequent_value_count:
                    frequent_value_count[rally[tactic_index + hit_id][attr_id]] = 0
                frequent_value_count[rally[tactic_index + hit_id][attr_id]] += 1
        frequent_value = {k: v for k, v in sorted(frequent_value_count.items(), key=lambda item: item[1])[::-1]}
        insert_tactic = deepcopy(tactic.tactic)
        insert_tactic[hit_id][attr_id] = list(frequent_value.keys())[0]
        return [insert_tactic]
