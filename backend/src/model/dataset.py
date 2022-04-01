from typing import List, Tuple, Union
from pydantic import BaseModel


class MatchInfo(BaseModel):
    name: str
    players: Tuple[Union[Tuple[str], Tuple[str, str]], Union[Tuple[str], Tuple[str, str]]]  # 双打或单打
    sequenceCount: int


class DatasetInfo(BaseModel):
    name: str
    matches: List[MatchInfo]
    attrs: List[str]


class SequenceFilter(BaseModel):
    dataset: str
    player: str
    opponents: List[str]
