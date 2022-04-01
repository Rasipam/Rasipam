import pdb
from typing import List, Dict

from pydantic import BaseModel

from src.model.hit import HitWithoutFrequency, HitWithoutFrequencyNoNone


class RallyDetail(BaseModel):
    hits: List[HitWithoutFrequency]


class Rally(BaseModel):
    id: int  # 唯一标识，直接用筛选出来的数据集中这个回合的序号就行
    win: bool  # 选定球员是不是赢了
    is_server: bool  # 选定球员是不是发球方
    hit_count: int  # 总共多少拍

    # 战术信息
    index: Dict[str, List[int]]  # 战术是从第几拍开始用的 {tactic_id: [index_in_rally]}

    rally: List[HitWithoutFrequencyNoNone]

    # 比赛信息，如果没有就随便填个
    match_name: str  # 比赛名
    video_name: str  # 比赛视频名
    start_time: float  # 回合开始时间
    end_time: float  # 回合结束时间

    @classmethod
    def find_for_tactic(cls, tactic_uuid, tactic_set, tactics, rallies):
        rally_list = []
        tactic_index = [i for i in range(len(tactic_set.tactics)) if tactic_set.tactics[i].id == tactic_uuid][0]
        for rally_id in range(len(rallies["sequences"])):
            if tactic_index not in [p_in_s["pattern_index"] for p_in_s in tactics["patterns_in_sequences"][rally_id]]:
                continue

            rally_index = {}
            winner = rallies["sequences"][rally_id]["winner"]
            server = rallies["sequences"][rally_id]["server"]

            match_name = ""

            id_top = 0
            for match in rallies["matches"]:
                id_top += match["sequence_number"]
                if rally_id < id_top:
                    match_name = match["match"][:match["match"].rfind('-')]
                    break

            for id_in_rally in range(len(tactics["patterns_in_sequences"][rally_id])):
                tactic_id = tactics["patterns_in_sequences"][rally_id][id_in_rally]["pattern_index"]
                tactic_pos_in_rally = tactics["patterns_in_sequences"][rally_id][id_in_rally]["pattern_position"]

                # tactic index
                try:
                    if tactic_id not in rally_index:
                        rally_index[tactic_set.tactics[tactic_id].id] = []
                    rally_index[tactic_set.tactics[tactic_id].id].append(tactic_pos_in_rally)
                except Exception as e:
                    pdb.set_trace()

            rally = cls(id=rally_id,
                        win=(winner == 0),
                        is_server=(server == 0),
                        hit_count=len(rallies["sequences"][rally_id]["events"]),
                        index=rally_index,
                        rally=rallies["sequences"][rally_id]["events"],
                        match_name=match_name,
                        video_name="",
                        start_time=0.0,
                        end_time=0.0
                        )  # TODO: add video info
            rally_list.append(rally)

        return rally_list
