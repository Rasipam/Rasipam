from typing import List, Tuple, Union, ClassVar, Dict
from uuid import uuid4

from pydantic import BaseModel

from src.model.hit import HitWithoutFrequency, HitWithFrequency
from src.utils.common import NR_ATTR


class TacticDetail(BaseModel):
    hits: List[HitWithoutFrequency]


class TacticSurroundingDetail(BaseModel):
    hits: List[HitWithFrequency]


class Tactic(BaseModel):
    # 唯一表示
    id: str  # gen by str(uuid4())

    # 具体战术
    user: int  # tactic第一拍的击球方，0或1
    tactic: List[HitWithoutFrequency]  # 如果是数组，这里可以改成List[HitDetail]
    tactic_surrounding: List[HitWithFrequency]

    # 使用统计
    seq_count: int  # 多少个回合使用了该战术（如果一个回合多次使用，只记一次）
    win_seq_count: int  # 使用该战术的回合，多少个赢了（同上）
    usage_count: int  # 该战术用了多少次（如果一个回合多次使用，记多次）
    win_usage_count: int  # 使用该战术，多少次赢了（同上）

    # 投影，0~1
    x: float
    y: float

    # 回合序号
    # index: List[Tuple[int, List[int]]]  # [(rally_id, index_in_rally)] or [(rally_id, [index_in_rally])]
    index: Dict[int, List[int]]  # {rally_id: [index_in_rally]}


class TacticSet(BaseModel):
    tactics: List[Tactic]
    desc_len: int

    surrounding_step: ClassVar[int] = 1

    @staticmethod
    def set_surrounding_step(surrounding_step):
        TacticSet.surrounding_step = surrounding_step

    @classmethod
    def load(cls, tactics, rallies, coordinates, desc_len, preference_tactics, last_tactics=None, nr_attr=NR_ATTR):
        if last_tactics is None:
            last_tactics = []
        _tactics = []
        tactics_in_rallies = tactics["patterns_in_sequences"]
        tactics_hits = tactics["patterns"]

        tactics_surrounding = [[
            [{}] * nr_attr if hit_id not in range(len(tac["hits"])) else
            [{} if v == "" else None for v in tac["hits"][hit_id]]
            for hit_id in range(-TacticSet.surrounding_step, len(tac["hits"]) + TacticSet.surrounding_step)] for tac in tactics_hits]
        tactics_index = [{} for _ in tactics_hits]

        seq_count_for_tactics = [0 for _ in tactics_hits]
        win_seq_count_for_tactics = [0 for _ in tactics_hits]
        usage_count_for_tactics = [0 for _ in tactics_hits]
        win_usage_count_for_tactics = [0 for _ in tactics_hits]

        for rally_id in range(len(rallies["sequences"])):
            rally = rallies["sequences"][rally_id]
            winner = rally["winner"]
            _seq_count_for_tactics = [0 for _ in tactics_hits]
            _win_seq_count_for_tactics = [0 for _ in tactics_hits]
            _usage_count_for_tactics = [0 for _ in tactics_hits]
            _win_usage_count_for_tactics = [0 for _ in tactics_hits]

            for id_in_rally in range(len(tactics_in_rallies[rally_id])):
                tactic_id_in_rally = tactics_in_rallies[rally_id][id_in_rally]["pattern_index"]
                tactic_pos_in_rally = tactics_in_rallies[rally_id][id_in_rally]["pattern_position"]
                tactic = tactics_hits[tactic_id_in_rally]
                tactic_surrounding = tactics_surrounding[tactic_id_in_rally]
                tactic_index = tactics_index[tactic_id_in_rally]

                _seq_count_for_tactics[tactic_id_in_rally] |= 1
                _win_seq_count_for_tactics[tactic_id_in_rally] |= 1 - winner
                _usage_count_for_tactics[tactic_id_in_rally] += 1
                _win_usage_count_for_tactics[tactic_id_in_rally] += 1 - winner

                # tactic surrounding
                for pos_in_rally in range(max(0, tactic_pos_in_rally - TacticSet.surrounding_step),
                                          min(tactic_pos_in_rally + len(tactic["hits"]) + TacticSet.surrounding_step,
                                              len(rally["events"]))):
                    for attr_id in range(len(rally["events"][pos_in_rally])):
                        pos_in_tactic = pos_in_rally - (tactic_pos_in_rally - TacticSet.surrounding_step)
                        surrounding_attr = tactic_surrounding[pos_in_tactic][attr_id]
                        if surrounding_attr is not None:
                            if rally["events"][pos_in_rally][attr_id] not in surrounding_attr:
                                surrounding_attr[rally["events"][pos_in_rally][attr_id]] = 0
                            surrounding_attr[rally["events"][pos_in_rally][attr_id]] += 1

                # tactic index
                if rally_id not in tactic_index:
                    tactic_index[rally_id] = []
                tactic_index[rally_id].append(tactic_pos_in_rally)

            seq_count_for_tactics = [c + _c for c, _c in zip(seq_count_for_tactics, _seq_count_for_tactics)]
            win_seq_count_for_tactics = [c + _c for c, _c in zip(win_seq_count_for_tactics, _win_seq_count_for_tactics)]
            usage_count_for_tactics = [c + _c for c, _c in zip(usage_count_for_tactics, _usage_count_for_tactics)]
            win_usage_count_for_tactics = [c + _c for c, _c in zip(win_usage_count_for_tactics, _win_usage_count_for_tactics)]

        for tactic_id in range(len(tactics_hits)):
            tactics_pool = last_tactics + preference_tactics
            find_tactic_prefered = [tac for tac in tactics_pool if tac.tactic == tactics_hits[tactic_id]["hits"]]
            if len(find_tactic_prefered) == 0:
                _tactics.append(Tactic(id=str(uuid4()), user=0,
                                       tactic=tactics_hits[tactic_id]["hits"],
                                       tactic_surrounding=tactics_surrounding[tactic_id],
                                       seq_count=seq_count_for_tactics[tactic_id],
                                       win_seq_count=win_seq_count_for_tactics[tactic_id],
                                       usage_count=usage_count_for_tactics[tactic_id],
                                       win_usage_count=win_usage_count_for_tactics[tactic_id],
                                       index=tactics_index[tactic_id],
                                       x=coordinates[tactic_id][0],
                                       y=coordinates[tactic_id][1]),
                                )
            else:
                _tactics.append(Tactic(id=find_tactic_prefered[0].id, user=0,
                                       tactic=tactics_hits[tactic_id]["hits"],
                                       tactic_surrounding=tactics_surrounding[tactic_id],
                                       seq_count=seq_count_for_tactics[tactic_id],
                                       win_seq_count=win_seq_count_for_tactics[tactic_id],
                                       usage_count=usage_count_for_tactics[tactic_id],
                                       win_usage_count=win_usage_count_for_tactics[tactic_id],
                                       index=tactics_index[tactic_id],
                                       x=coordinates[tactic_id][0],
                                       y=coordinates[tactic_id][1]),
                                )

        tactic_set = cls(tactics=_tactics, desc_len=desc_len)
        return tactic_set
