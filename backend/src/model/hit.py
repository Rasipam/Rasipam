from typing import List, Union

from src.model.value import Value, FreqValue


Hit = List[Union[Value, FreqValue, None]]

HitWithFrequency = List[Union[FreqValue, None]]

HitWithoutFrequency = List[Union[Value, None]]

HitNoNone = List[Union[Value, FreqValue]]

HitWithFrequencyNoNone = List[FreqValue]

HitWithoutFrequencyNoNone = List[Value]
