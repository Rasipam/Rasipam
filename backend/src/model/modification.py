from typing import Dict, Union, Any

from pydantic import BaseModel


class Modification(BaseModel):
    type: str
    params: Dict[str, Union[str, Any]]


all_modification_types = [
    {
        "objective": 'TacticSet',  # 战术集
        "types": [
            {
                "type": 'LimitIndex',  # 战术位置
                "params": ['min', 'max'],  # 最小拍序号，最大拍序号
            },
            {
                "type": 'LimitLength',  # 战术长度
                "params": ['min', 'max', 'offset'],  # 最小长度，最大长度，相对当前长度的变化（1表示更长，-1表示更短）
            },
        ]
    },
    {
        "objective": 'Attribute',  # 属性
        "type": [
            {
                "type": 'SetExistence',  # 是否考虑一个属性
                "params": ['attr', 'exist'],  # 属性名，是否考虑
            },
            {
                "type": 'SetImportance',  # 重要性
                "params": ['attr', 'importance'],  # 属性名，属性重要度变化（1或-1）
            },
        ]
    },
    {
        "objective": 'Tactic',  # 战术
        "type": [
            {
                "type": 'Delete',  # 删除
                "params": ['index'],  # 战术id
            },
            {
                "type": 'Split',  # 拆分
                "params": ['index', 'attr', 'hit'],  # 战术id，根据属性拆分，根据值拆分（最后两个二选一）
            },
            {
                "type": 'Merge',  # 合并
                "params": ['index'],  # 战术id
            },
        ]
    },
    {
        "objective": 'Hit',  # 单个战术中的拍
        "type": [
            {
                "type": 'Increment',  # 在首尾增加拍
                "params": ['index', 'direction', 'hitCount'],  # 战术id，首还是尾（0首1尾），增加拍数
            },
            {
                "type": 'Decrement',  # 在首尾减去拍
                "params": ['index', 'direction', 'hitCount'],  # 战术id，首还是尾（0首1尾），减少拍数
            },
        ]
    },
    {
        "objective": 'Value',  # 单个战术中的值
        "type": [
            {
                "type": 'Replace',  # 替换
                "params": ['index', 'hit', 'attr', 'target'],  # 战术序号，战术中的拍序号，属性名，目标值
            },
            {
                "type": 'Ignore',  # 忽略值
                "params": ['index', 'hit', 'attr'],  # 战术序号，拍序号，属性名
            },
            {
                "type": 'Explore',  # 明确值
                "params": ['index', 'hit', 'attr'],  # 战术序号，拍序号，属性名
            },
        ]
    }
]

global_modification_types = ['LimitIndex', 'LimitLength', 'SetExistence', 'SetImportance']

local_modification_types = ['Delete', 'Split', 'Merge', 'Increment', 'Decrement', 'Replace', 'Ignore', 'Explore']
